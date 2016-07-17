// Copyright (C), 1998-2013, Tencent
// Author: jefftang@tencent.com
// Date: 2013-1-17

#include "common/system/concurrency/threadworker.h"

#include <fcntl.h>
#include <signal.h>
#include <stdint.h>
#include <syscall.h>
#include <sys/epoll.h>

#include <list>
#include <string>
#include <vector>

#include "app/qzap/common/base/base.h"  // arraysize
#include "common/base/scoped_ptr.h"
#include "common/system/concurrency/atomic/atomic.h"

#include "thirdparty/glog/logging.h"

extern "C" {
static inline int eventfd(int count, int flags) {
  return syscall(290, count, flags);  // eventfd 290
}
}
namespace {
static __thread weak_ptr<gdt::ThreadWorker> * volatile thread_weak_worker = 0;
}

namespace gdt {

struct ThreadWorker::Impl {
 public:
  Impl(const std::string &name, ThreadWorker *worker)
    : name_(name), eventfd_(-1),
      running_(false), shutdown_(true),
      worker_(worker), cpu_(-1) {
    // TODO(naicaisun): check whether can be replaced by gdt::SpinLock,
    // gdt::MutexBase and gdt::ConditionVariable
    pthread_spin_init(&push_task_spin_lock_, 0);
    pthread_mutex_init(&mutex_, NULL);
    pthread_cond_init(&thread_done_cond_, NULL);
    push_task_ = new std::vector<gdt::Closure*>;
    run_task_ = new std::vector<gdt::Closure*>;
  }

  ~Impl() {
    Stop();
    pthread_spin_destroy(&push_task_spin_lock_);
    pthread_mutex_destroy(&mutex_);
    pthread_cond_destroy(&thread_done_cond_);
    if (push_task_) {
      delete push_task_;
      push_task_ = NULL;
    }
    if (run_task_) {
      delete run_task_;
      run_task_ = NULL;
    }
  }

  void set_cpu(int32_t cpu) {
    cpu_ = cpu;
  }

  int32_t cpu() const {
    return cpu_;
  }

  bool PushTask(gdt::Closure *t) {
    pthread_spin_lock(&push_task_spin_lock_);
    if (!running_) {
      VLOG(2) << name_ << " alread stopped";
      pthread_spin_unlock(&push_task_spin_lock_);
      return false;
    }
    if (shutdown_) {
      pthread_spin_unlock(&push_task_spin_lock_);
      return false;
    }
    bool is_empty = push_task_->empty();
    push_task_->push_back(t);
    pthread_spin_unlock(&push_task_spin_lock_);
    if (is_empty) {
      NewTaskNotify();
    }
    return true;
  }

  void Start() {
    pthread_mutex_lock(&mutex_);
    pthread_spin_lock(&push_task_spin_lock_);
    if (running_) {
      LOG(WARNING) << "TickWorker:" << name_ << " already running";
      pthread_spin_unlock(&push_task_spin_lock_);
      pthread_mutex_unlock(&mutex_);
      return;
    }
    CHECK(push_task_->empty());
    CHECK(shutdown_);
    running_ = true;
    shutdown_ = false;
    pthread_spin_unlock(&push_task_spin_lock_);
    efd_ = epoll_create(128);
    if (efd_ < 0) {
      LOG(ERROR) << "Fail to epoll_create, error: " << strerror(errno);
      pthread_mutex_unlock(&mutex_);
      return;
    }
    eventfd_ = eventfd(0, 0);  // EFD_NONBLOCK is since 2.6.27, so replace it?
    if (eventfd_ < 0) {
      LOG(ERROR) << "Fail to eventfd, error: " << strerror(errno);
      close(efd_);
      efd_ = -1;
      pthread_mutex_unlock(&mutex_);
      return;
    }
    int oldflags = fcntl(eventfd_, F_GETFL, 0);
    if (oldflags < 0) {
      LOG(ERROR) << "Unable to read fd: " << efd_ << " flags";
      close(efd_);
      close(eventfd_);
      pthread_mutex_unlock(&mutex_);
      return;
    }
    oldflags |= O_NONBLOCK;
    if (fcntl(eventfd_, F_SETFL, oldflags) != 0) {
      close(efd_);
      close(eventfd_);
      pthread_mutex_unlock(&mutex_);
      LOG(ERROR) << "Unable to set fd " << eventfd_ << " flags";
      return;
    }
    struct epoll_event ev;
    memset(&ev, 0, sizeof(ev));
    ev.events = EPOLLIN | EPOLLPRI | EPOLLET;
    int ret = epoll_ctl(efd_, EPOLL_CTL_ADD, eventfd_, &ev);
    if (ret < 0) {
      LOG(ERROR) << "epoll_ctl fail to add fd: " << eventfd_;
      close(efd_);
      close(eventfd_);
      pthread_mutex_unlock(&mutex_);
      return;
    }

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    ret = pthread_create(
        &thread_id_, &attr, InternalRun, this);
    if (ret != 0) {
      LOG(ERROR) << "ThreadWorker: " << name_ << " fail to create pthread";
      pthread_mutex_unlock(&mutex_);
      return;
    }
    VLOG(2) << "ThreadWorker: " << name_ << " start";
    pthread_mutex_unlock(&mutex_);
  }

  void Stop() {
    pthread_mutex_lock(&mutex_);
    pthread_spin_lock(&push_task_spin_lock_);
    if (!running_) {
      VLOG(2) << "ThreadWorker: " << name_ << " already stop";
      pthread_spin_unlock(&push_task_spin_lock_);
      pthread_mutex_unlock(&mutex_);
      return;
    }
    shutdown_ = true;
    pthread_spin_unlock(&push_task_spin_lock_);
    NewTaskNotify();
    while (running_) {
      pthread_cond_wait(&thread_done_cond_, &mutex_);
    }
    CHECK(!running_);
    close(efd_);
    close(eventfd_);
    eventfd_ = -1;
    efd_ = -1;
    pthread_mutex_unlock(&mutex_);
  }

 private:
  void NewTaskNotify() {
    uint64_t u = 1;
    if (write(eventfd_, &u, sizeof(u)) != sizeof(u)) {
      LOG(ERROR) << "ThreadWorker: " << name_ << " write eventfd: " << eventfd_
        << " error:" << strerror(errno);
    }
  }

  void Run() {
    VLOG(2) << "ThreadWorker: " << name_ << " running";
    struct epoll_event ev[128];
    sigset_t sset;
    sigemptyset(&sset);
    sigaddset(&sset, SIGPIPE);
    pthread_sigmask(SIG_BLOCK, &sset, NULL);
    while (1) {
      int ret = epoll_wait(efd_, ev, arraysize(ev),
                           -1);
      if (ret == 0) {
        continue;
      } else if (ret == -1) {
        if (errno == EINTR) {
          continue;
        }
        LOG(ERROR) << "epoll_wait error:" << strerror(errno);
        break;
      }
      uint64_t u = 0;
      if (read(eventfd_, &u, sizeof(u)) != sizeof(u)) {
        LOG(ERROR) << "ThreadWorker: " << name_ << " read eventfd: " << eventfd_
          << " error:" << strerror(errno);
        break;
      }
      VLOG(2) << "ThreadWorker: " << name_ << " get tasks to run";
      RunTask();
      if (shutdown_) {
        RunTask();
        break;
      }
    }
    VLOG(2) << "ThreadWorker: " << name_ << " Stop";
    volatile weak_ptr<ThreadWorker> *weak_worker =
        AtomicExchange(&thread_weak_worker,
                       static_cast<weak_ptr<ThreadWorker>*>(NULL));
    if (weak_worker != NULL) {
      delete weak_worker;
    }
    pthread_mutex_lock(&mutex_);
    running_ = false;
    pthread_cond_broadcast(&thread_done_cond_);
    pthread_mutex_unlock(&mutex_);
  }

  void RunTask() {
    std::vector<gdt::Closure *> *tmp = run_task_;
    run_task_ = push_task_;
    pthread_spin_lock(&push_task_spin_lock_);
    push_task_ = tmp;
    pthread_spin_unlock(&push_task_spin_lock_);
    std::vector<gdt::Closure*> &p = *run_task_;
    for (size_t i = 0; i < p.size(); ++i) {
      p[i]->Run();
    }
    run_task_->clear();
  }

  static void *InternalRun(void *arg) {
    ThreadWorker::Impl *impl = reinterpret_cast<ThreadWorker::Impl*>(arg);
    int32_t cpuno = impl->cpu();
    if (cpuno >= 0) {
      cpu_set_t mask;
      CPU_ZERO(&mask);
      CPU_SET(cpuno, &mask);
      int ret = sched_setaffinity(0, sizeof(mask), &mask);
      CHECK_EQ(ret, 0) << "sched_setaffinity error: " << strerror(errno);
    }
    weak_ptr<ThreadWorker> *weak_worker = new
      weak_ptr<ThreadWorker>(impl->weak_executor_);
    weak_ptr<ThreadWorker>* ret =
        AtomicExchange(&thread_weak_worker, weak_worker);
    CHECK_EQ(ret, static_cast<weak_ptr<ThreadWorker>*>(NULL))
        << "WeakThreadWorker is not null";
    impl->Run();
    return NULL;
  }

  std::string name_;
  int eventfd_;
  int efd_;
  pthread_mutex_t mutex_;
  pthread_cond_t thread_done_cond_;
  pthread_t thread_id_;
  volatile bool running_;
  bool shutdown_;
  ThreadWorker *worker_;
  int32_t cpu_;

  std::vector<gdt::Closure*> *push_task_;
  std::vector<gdt::Closure*> *run_task_;
  pthread_spinlock_t push_task_spin_lock_;
  weak_ptr<ThreadWorker> weak_executor_;
  friend class ThreadWorker;
};

ThreadWorker::ThreadWorker(const std::string &name)
  : impl_(new Impl(name, this)) {
}

ThreadWorker::~ThreadWorker() {
  delete impl_;
}

void ThreadWorker::Start() {
  impl_->Start();
}

void ThreadWorker::Stop() {
  impl_->Stop();
}

bool ThreadWorker::PushTask(gdt::Closure *t) {
  return impl_->PushTask(t);
}

shared_ptr<ThreadWorker> ThreadWorker::Create(
    const std::string &name) {
  shared_ptr<ThreadWorker> p(new ThreadWorker(name));
  p->impl_->weak_executor_ = p;
  return p;
}

shared_ptr<ThreadWorker> ThreadWorker::current_executor() {
  shared_ptr<ThreadWorker> executor;
  weak_ptr<ThreadWorker> *volatile weak_worker = thread_weak_worker;
  if (weak_worker == NULL) {
    return executor;
  }
  executor = weak_worker->lock();
  return executor;
}

void ThreadWorker::set_cpu(
    int32_t cpu) {
  impl_->set_cpu(cpu);
}

}  // namespace gdt
