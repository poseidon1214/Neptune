// Copyright (C), 1998-2013, Tencent
// Author: jefftang@tencent.com
// Date: 2013-1-17

#ifndef COMMON_SYSTEM_CONCURRENCY_THREADPOOL_H_
#define COMMON_SYSTEM_CONCURRENCY_THREADPOOL_H_

#include <list>
#include <string>
#include <vector>

#include "common/base/shared_ptr.h"
#include "common/base/string/algorithm.h"
#include "common/system/concurrency/atomic/atomic.h"
#include "common/system/concurrency/condition_variable.h"
#include "common/system/concurrency/mutex.h"
#include "common/system/concurrency/thread.h"

#include "thirdparty/glog/logging.h"

namespace gdt {
// The thread pool implement base on the pcqueue.
class ThreadPool {
 public:
  static shared_ptr<ThreadPool> Create(const std::string& name, int size) {
    std::vector<int> cpu;
    shared_ptr<ThreadPool> p(new ThreadPool(name, size, cpu));
    return p;
  }

  static shared_ptr<ThreadPool> CreatePool(
      const std::string& name, int size, std::vector<int> cpu) {
    shared_ptr<ThreadPool> p(new ThreadPool(name, size, cpu));
    return p;
  }

  virtual ~ThreadPool() {
    Stop();
    if (thread_init_) {
      delete thread_init_;
      thread_init_ = NULL;
    }
  }

  void RegisterThreadInit(Closure *thread_init) {
    if (running()) {
      LOG(WARNING) << "register failed for thread pool has been starting.";
      return;
    }
    thread_init_ = thread_init;
  }

  void Start() {
    VLOG(50) << name() << " start, size:" << size_;
    gdt::MutexLocker locker(&mutex_);
    if (running()) {
      LOG(WARNING) << name() << " already running.";
      return;
    }
    while (worker_number_ > 0) {
      LOG(WARNING) << "Previous start is not shutdown clearly.";
      all_worker_done_.Wait(&mutex_);
    }
    CHECK(task_.empty());
    running_ = true;
    worker_number_ = size_;
    threads_.clear();
    for (int i = 0; i < size_; ++i) {
      gdt::ThreadAttributes attr;
      attr.SetName(StringPrintf("%s_%d", name_.c_str(), i));
      attr.SetDetached(true);
      if (!cpu_.empty() && cpu_[i % cpu_.size()] >= 0) {
        attr.SetAffinity(cpu_[i % cpu_.size()]);
      }
      shared_ptr<gdt::Thread> t(new gdt::Thread(attr,
          NewCallback(this, &ThreadPool::Loop,
                      name_.empty() ?
                          strdup("NoName") :
                          strdup(name_.c_str()), i)));
      threads_.push_back(t);
      t->Start();
    }
  }

  void Stop() {
    VLOG(50) << "ThreadPool " << name() << " Stop.";
    gdt::MutexLocker locker(&mutex_);
    if (running()) {
      running_ = false;
      new_task_notify_.NotifyAll();
    }
    while (worker_number_ > 0) {
      VLOG(50) << "wait all worker done.";
      all_worker_done_.Wait(&mutex_);
    }
  }

  bool running() {
    return running_;
  }

  const std::string &name() const {
    return name_;
  }

  int size() const {
    return size_;
  }

  virtual bool PushTask(Closure *t) {
    CHECK(t != NULL) << name() <<  " can't push null task.";
    gdt::MutexLocker locker(&mutex_);
    if (!running()) {
      LOG(WARNING) << name() << " already stopped";
      return false;
    }
    task_.push_back(t);
    new_task_notify_.Notify();
    return true;
  }

 protected:
  ThreadPool(const std::string& name, int size, std::vector<int> cpu)
    : thread_init_(NULL), size_(size), cpu_(cpu), running_(false),
      worker_number_(0), name_(name) {
  }

  virtual void Loop(char *pool_name, int i) {
    VLOG(50) << pool_name << " worker " << i << " start.";
    if (thread_init_) {
      thread_init_->Run();
    }
    bool continued = true;
    while (continued) {
      Closure *p = NULL;
      mutex_.Lock();
      while (task_.empty() && running()) {
        new_task_notify_.Wait(&mutex_);
      }
      if (!task_.empty()) {
        p = task_.front();
        task_.pop_front();
      }
      continued = running() || !task_.empty();
      mutex_.Unlock();
      if (p) {
        p->Run();
      }
    }
    VLOG(50) << pool_name << " woker " << i << " stop";
    free(pool_name);
    if (AtomicDecrement(&worker_number_) == 0) {
      all_worker_done_.Notify();
    }
  }

 private:
  typedef std::list<Closure*> TaskList;
  Closure* thread_init_;
  std::vector<shared_ptr<gdt::Thread> > threads_;
  int size_;
  std::vector<int> cpu_;
  bool running_;
  CondVar new_task_notify_;
  CondVar all_worker_done_;
  volatile intptr_t worker_number_;
  TaskList task_;
  gdt::Mutex mutex_;
  std::string name_;
  DISALLOW_COPY_AND_ASSIGN(ThreadPool);
};

}  // namespace gdt

#endif  // COMMON_SYSTEM_CONCURRENCY_THREADPOOL_H_
