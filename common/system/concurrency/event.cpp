// Copyright (c) 2011, Tencent Inc. All rights reserved.
// Author: CHEN Feng <phongchen@tencent.com>
// Created: 2010-06-18

#include "common/system/concurrency/event.h"
#include <stdlib.h>
#include "common/system/check_error.h"

#include "common/system/concurrency/atomic/atomic.h"
#include "common/system/system_information.h"

namespace gdt {

//////////////////////////////////////////////////////////////////////////////
// AutoResetEvent staff

AutoResetEvent::AutoResetEvent(bool init_state) : m_signaled(init_state) {
}

AutoResetEvent::~AutoResetEvent() {
}

void AutoResetEvent::Wait() {
  MutexLocker locker(m_mutex);
  while (!m_signaled)
    m_cond.Wait(&m_mutex);
  m_signaled = false;
}

bool AutoResetEvent::TimedWait(int64_t timeout_ms) {
  MutexLocker locker(m_mutex);
  if (!m_signaled)
    m_cond.TimedWait(&m_mutex, timeout_ms);

  if (!m_signaled)
    return false;

  m_signaled = false;
  return true;
}

bool AutoResetEvent::TryWait() {
  return TimedWait(0);
}

void AutoResetEvent::Set() {
  MutexLocker locker(m_mutex);
  m_signaled = true;
  m_cond.Signal();
}

void AutoResetEvent::Reset() {
  MutexLocker locker(m_mutex);
  m_signaled = false;
}

//////////////////////////////////////////////////////////////////////////////
// ManualResetEvent staff

ManualResetEvent::ManualResetEvent(bool init_state) : m_signaled(init_state) {
}

ManualResetEvent::~ManualResetEvent() {
}

void ManualResetEvent::Wait() {
  MutexLocker locker(m_mutex);
  while (!m_signaled)
    m_cond.Wait(&m_mutex);
}

bool ManualResetEvent::TimedWait(int64_t timeout_ms) {
  MutexLocker locker(m_mutex);
  if (!m_signaled)
    m_cond.TimedWait(&m_mutex, timeout_ms);
  return m_signaled;
}

bool ManualResetEvent::TryWait() {
  return TimedWait(0);
}

void ManualResetEvent::Set() {
  MutexLocker locker(m_mutex);
  m_signaled = true;
  m_cond.Broadcast();
}

void ManualResetEvent::Reset() {
  MutexLocker locker(m_mutex);
  m_signaled = false;
}

}  // namespace gdt

