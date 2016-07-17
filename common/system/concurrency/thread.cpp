// Copyright (c) 2012, Tencent Inc.
// All rights reserved.
//
// Author: CHEN Feng <phongchen@tencent.com>
// Created: 01/13/12
// Description: Thread implementation

#include "common/system/concurrency/thread.h"

#include <string>
#include "common/system/check_error.h"
#include "common/system/concurrency/base_thread.h"

#include "thirdparty/glog/logging.h"

namespace gdt {

class Thread::Impl : public BaseThread {
 public:
  explicit Impl(const ThreadAttributes& attributes, Closure* closure = NULL)
    : BaseThread(attributes), m_closure(closure) {
  }

  explicit Impl(Closure* closure = NULL) : m_closure(closure) {
  }

  ~Impl() {
    delete m_closure;
    // Reset all to null for debug purpose
    m_closure = NULL;
  }

  void Initialize(Closure* closure) {
    CheckNotJoinable();

    if (closure != m_closure) {
      delete m_closure;
      m_closure = closure;
    }
  }

  bool IsInitialized() const {
    return m_closure != NULL;
  }

  void CheckNotJoinable() const {
    // Can't reinitialze a joinable thread
    if (IsJoinable())
      CHECK_ERRNO_ERROR(EINVAL);
  }

 private:
  virtual void Entry() {
    if (m_closure) {
      Closure* closure = m_closure;
      if (!closure->IsRepeatable())
        m_closure = NULL;
      closure->Run();
    }
  }

  virtual void OnExit() {
    if (!IsJoinable())
      delete this;
    else
      BaseThread::OnExit();
  }

 private:
  Closure* m_closure;
};


Thread::Thread(const ThreadAttributes& attr, Closure* closure)
  : pimpl_(new Impl(attr, closure)) {
}

Thread::Thread(const std::string& name, Closure* closure)
  : pimpl_(new Impl(ThreadAttributes().SetName(name), closure)) {
}

Thread::Thread(Closure* closure) : pimpl_(new Impl(closure)) {
}

Thread::~Thread() {
  delete pimpl_;
  pimpl_ = NULL;
}

void Thread::Initialize(Closure* closure) {
  pimpl_->Initialize(closure);
}

void Thread::SetAffinity(int cpu_index) {
  pimpl_->CheckNotJoinable();
  pimpl_->attributes_.SetAffinity(cpu_index);
}

void Thread::SetDetach() {
  pimpl_->CheckNotJoinable();
  pimpl_->attributes_.SetDetached(true);
}

bool Thread::Start() {
  if (!pimpl_->IsInitialized())
    CHECK_ERRNO_ERROR(EINVAL);
  bool detached = pimpl_->GetAttributes().IsDetached();
  bool result = pimpl_->Start();
  if (result && detached)
    pimpl_ = NULL;
  return result;
}

bool Thread::Join() {
  return pimpl_ && pimpl_->Join();
}

void Thread::Detach() {
  // After detached, the pimpl_ will be deleted in Thread::Impl::OnExit.
  // So don't delete it here, just set it be NULL to mark this thread object
  // to be detached.
  if (pimpl_) {
    pimpl_->DoDetach();
    pimpl_ = NULL;
  }
}

bool Thread::IsAlive() const {
  return pimpl_ && pimpl_->IsAlive();
}

bool Thread::IsJoinable() const {
  return pimpl_ && pimpl_->IsJoinable();
}

int Thread::Id() const {
  return pimpl_->Id();
}

}  // namespace gdt

