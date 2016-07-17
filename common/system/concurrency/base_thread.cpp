// Copyright (c) 2011, Tencent Inc. All rights reserved.

#include "common/system/concurrency/base_thread.h"

#include <assert.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/prctl.h>

#include <string>

#include "common/system/check_error.h"
#include "common/system/concurrency/atomic/atomic.h"
#include "thirdparty/glog/logging.h"


namespace gdt {

BaseThread::BaseThread():
  handle_(),
  id_(-1),
  alive_(false) {
}

BaseThread::BaseThread(const ThreadAttributes& attributes) :
  attributes_(attributes),
  handle_(),
  id_(-1),
  alive_(false) {
}

BaseThread::~BaseThread() {
  // What we're trying to do is allow the thread we want to delete to complete
  // running. So we wait for it to stop.
  if (IsJoinable())
    Join();
}

void BaseThread::OnExit() {
}

bool BaseThread::Start() {
  // Start can only be used on a new born or a started but already joined
  // or a detached Thread object. In other words, not associated to any
  // system thread, both alive and dead.
  if (IsJoinable()) {
    // If crash here, means the thread is still alive or finished but not
    // joined.
    CHECK_ERRNO_ERROR(EINVAL);
  }

  handle_ = ThreadHandleType();
  id_ = 0;
  bool detached = GetAttributes().IsDetached();
  pthread_t thread;
  pthread_t* pthread = detached ? &thread : &handle_;

  int error = pthread_create(pthread, &attributes_.attr_, StaticEntry, this);
  if (error != 0) {
    if (error != EAGAIN) {
      CHECK_PTHREAD_ERROR(error);
    }
    id_ = -1;
  }
  return error == 0;
}

bool BaseThread::DoDetach() {
  CHECK_PTHREAD_ERROR(pthread_detach(handle_));
  handle_ = HandleType();
  id_ = -1;
  return true;
}

bool BaseThread::Join() {
  assert(IsJoinable());
  CHECK_PTHREAD_ERROR(pthread_join(handle_, NULL));
  handle_ = pthread_t();
  id_ = -1;
  return true;
}

int BaseThread::Id() const {
  if (id_ != 0)
    return id_;

  // Id is rarely used, so busy wait is more fitness
  while (AtomicGet(&id_) == 0)
    ThisThread::Sleep(1);

  return id_;
}

bool BaseThread::IsAlive() const {
  return alive_;
}

bool BaseThread::IsJoinable() const {
  return !pthread_equal(handle_, pthread_t());
}

// make sure execute before exit
void BaseThread::Cleanup(void* param) {
  BaseThread* thread = static_cast<BaseThread*>(param);
  thread->alive_ = false;
  thread->OnExit();
}

void* BaseThread::StaticEntry(void* arg) {
  BaseThread* self = static_cast<BaseThread*>(arg);
  const ThreadAttributes& attr = self->GetAttributes();
  bool detached = attr.IsDetached();

  uint64_t affinity_mask = attr.AffinityMask();
  if (affinity_mask != 0) {
    cpu_set_t mask;
    CPU_ZERO(&mask);
    for (size_t i = 0; i < sizeof(affinity_mask) * CHAR_BIT; ++i) {
      if (affinity_mask & (1ULL << i))
        CPU_SET(i, &mask);
    }
    if (sched_setaffinity(0, sizeof(mask), &mask) < 0)
      PLOG(WARNING) << "Can't set affinity mask: " << std::hex << affinity_mask;
  }

  if (!detached) {
    self->alive_ = true;
    self->id_ = ThisThread::GetId();
  }
  const std::string& name = self->attributes_.name_;
  if (!name.empty()) {
    // Set thread name for easy debugging.
#if __GLIBC__ > 2 || __GLIBC__ == 2 && __GLIBC_MINOR__ >= 12
    pthread_setname_np(pthread_self(), name.c_str());
#else
    prctl(PR_SET_NAME, name.c_str(), 0, 0, 0);
#endif
  }
  pthread_cleanup_push(Cleanup, self);
  self->Entry();
  self->alive_ = false;
  pthread_cleanup_pop(true);

  return 0;
}

}  // namespace gdt
