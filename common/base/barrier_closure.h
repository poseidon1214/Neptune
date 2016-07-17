// Copyright (c) 2014, Tencent Inc.
// All rights reserved.
//
// Author: jaydentang@tencent.com

#ifndef COMMON_BASE_BARRIER_CLOSURE_H_
#define COMMON_BASE_BARRIER_CLOSURE_H_

#include "app/qzap/common/base/base.h"
#include "common/base/callback.h"
#include "common/system/concurrency/atomic/atomic.h"

namespace gdt {

class BarrierClosure : public Closure {
 public:
  BarrierClosure(int n, Closure* done)
    : left_(n), done_(done) { }

  ~BarrierClosure() { }
  virtual bool IsRepeatable() const { return false; }
  void Run() {
    if (AtomicDecrement(&left_) == 0) {
      done_->Run();
      delete this;
    }
  }

 private:
  int left_;
  Closure* done_;
  DISALLOW_COPY_AND_ASSIGN(BarrierClosure);
};

}  // namespace gdt

#endif  // COMMON_BASE_BARRIER_CLOSURE_H_

