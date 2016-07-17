// Copyright (c) 2011, Tencent Inc.
// All rights reserved.
//
// Author: CHEN Feng <phongchen@tencent.com>
// Created: 2011-08-31
// Description: thread local storage

#ifndef COMMON_SYSTEM_CONCURRENCY_THREAD_LOCAL_H
#define COMMON_SYSTEM_CONCURRENCY_THREAD_LOCAL_H

#include <pthread.h>
#include <stddef.h>
#include <stdint.h>

#include "common/base/uncopyable.h"
#include "app/qzap/common/base/callback.h"

// this module is the encapsulation of thread local storage
// include ThreadLocalValue object and ThreadLocalPtr smart pointer
// see http://en.wikipedia.org/wiki/Thread-local_storage for more details

namespace gdt {

namespace details {

// system specified details
class ThreadLocalSlot {
  DECLARE_UNCOPYABLE(ThreadLocalSlot);
 public:
  ThreadLocalSlot(void (*destructor)(void* p));
  ~ThreadLocalSlot();

  void* Get() const;
  void Set(void* p);

 private:
  pthread_key_t m_key;
};

} // end namespace details

struct ThreadLocalDefaultDeleter {
  template <typename T>
  static void Delete(T* p) {
    delete p;
  }
};

/// thread local object
/// example:
/// static ThreadLocalValue<int> n;
/// static ThreadLocalValue<int> n(1); // with construction param
/// static ThreadLocalValue<std::complex<int> > c(1.0, 2.0); // with more construction params
template <typename T, typename Deleter = ThreadLocalDefaultDeleter>
class ThreadLocalValue {
  typedef ThreadLocalValue ThisType;
 public:
  // support multiple params ctors
  ThreadLocalValue() : m_slot(&Delete) {
    T* (*creator)() = &ThisType::New;
    m_creator = NewPermanentCallback(creator);
  }

  template <typename A1>
  explicit ThreadLocalValue(A1 a1) : m_slot(&Delete) {
    T* (*creator)(A1) = &New<A1>;
    m_creator = NewPermanentCallback(creator, a1);
  }

  template <typename A1, typename A2>
  ThreadLocalValue(A1 a1, A2 a2) : m_slot(&Delete) {
    T* (*creator)(A1, A2) = &New<A1, A2>;
    m_creator = NewPermanentCallback(creator, a1, a2);
  }

  template <typename A1, typename A2, typename A3>
  ThreadLocalValue(A1 a1, A2 a2, A3 a3) : m_slot(&Delete) {
    T* (*creator)(A1, A2, A3) = &New<A1, A2, A3>;
    m_creator = NewPermanentCallback(creator, a1, a2, a3);
  }

  template <typename A1, typename A2, typename A3, typename A4>
  ThreadLocalValue(A1 a1, A2 a2, A3 a3, A4 a4) : m_slot(&Delete) {
    T* (*creator)(A1, A2, A3, A4) = &New<A1, A2, A3, A4>;
    m_creator = NewPermanentCallback(creator, a1, a2, a3, a4);
  }

  ~ThreadLocalValue() {
    delete m_creator;
    m_creator = NULL;
  }

 public: // accessors
  T& Value() {
    T* p = static_cast<T*>(m_slot.Get());
    if (!p) {
      p = m_creator->Run();
      m_slot.Set(p);
    }
    return *p;
  }

  const T& Value() const {
    return const_cast<ThreadLocalValue*>(this)->Value();
  }

  /// obtain address of holding object
  const T* Address() const {
    return &Value();
  }

  /// obtain address of holding object
  T* Address() {
    return &Value();
  }

  T& operator=(const T& value) {
    return (Value() = value);
  }

  T& operator=(const ThisType& rhs) {
    return (Value() = rhs.Value());
  }

  template <typename U>
  T& operator=(const ThreadLocalValue<U>& rhs) {
    return (Value() = rhs.Value());
  }

 private:
  static T* New() {
    return new T();
  }

  template <typename A1>
  static T* New(A1 a1) {
    return new T(a1);
  }

  template <typename A1, typename A2>
  static T* New(A1 a1, A2 a2) {
    return new T(a1, a2);
  }

  template <typename A1, typename A2, typename A3>
  static T* New(A1 a1, A2 a2, A3 a3) {
    return new T(a1, a2, a3);
  }

  template <typename A1, typename A2, typename A3, typename A4>
  static T* New(A1 a1, A2 a2, A3 a3, A4 a4) {
    return new T(a1, a2, a3, a4);
  }

  static void Delete(void* p) {
    Deleter::Delete(static_cast<T*>(p));
  }

 private:
  details::ThreadLocalSlot m_slot;
  Callback<T*()>* m_creator;
};

class ThreadLocal {
 public:
  enum CreatePolicy {
    // Avoid 0, C++ allow any compile time constant integral expression
    // with value 0 as null pointer/
    AUTO_CREATE = 1
  };
};

/// thread local smart pointer
/// example:
/// static ThreadLocalPtr<int> p;
/// static ThreadLocalPtr<int> p(new int(1)); // with prototype
/// static ThreadLocalPtr<int> p(ThreadLocal::AUTO_CREATE); // with auto create
template <typename T, typename Deleter = ThreadLocalDefaultDeleter>
class ThreadLocalPtr {
 private:
  typedef details::ThreadLocalSlot ThreadLocalSlot;
 public:
  typedef T ElementType;

 public:
  // the default constructed value is null
  ThreadLocalPtr() :
    m_slot(&Delete), m_prototype(NULL), m_getter(GetWithoutPrototype) {
  }

  // the default value is null, but you can give a prototype, and
  // ThreadLocalPtr will copy it as the default value
  explicit ThreadLocalPtr(T* prototype) :
    m_slot(&Delete), m_prototype(prototype), m_getter(GetWithPrototype) {
  }

  // pointer will be auto created at the first access time
  explicit ThreadLocalPtr(ThreadLocal::CreatePolicy) :
    m_slot(&Delete), m_prototype(NULL), m_getter(GetWithAutoCreate) {
  }

  ~ThreadLocalPtr() {
    delete m_prototype;
    m_prototype = NULL;
  }

 public:
  /// get holding value
  T* Get() const {
    return m_getter(&m_slot, m_prototype);
  }

  T* operator->() const {
    return Get();
  }
  T& operator*() const {
    return *Get();
  }

  /// set new value, release old value if any
  void Reset(T* new_value = 0) {
    T* p = static_cast<T*>(m_slot.Get());
    if (p != new_value)
      Deleter::Delete(p);
    m_slot.Set(new_value ? new_value : InvalidPtr());
  }

  /// detach holding value and return it
  T* Release() {
    T * p = Get();
    m_slot.Set(InvalidPtr());
    return p;
  }

 private:
  // default value is null, but if prototype is not null, should be create
  // from prototype.
  // so we need using kInvalidPtr to indicates the user set null value
  static T* InvalidPtr() {
    return reinterpret_cast<T*>(~static_cast<uintptr_t>(0));
  }

  static T* GetWithPrototype(ThreadLocalSlot* slot, const T* prototype) {
    T* p = static_cast<T*>(slot->Get());
    if (!p) {
      p = new T(*prototype);
      slot->Set(p);
    } else if (p == InvalidPtr()) {
      p = NULL;
    }
    return p;
  }

  static T* GetWithoutPrototype(ThreadLocalSlot* slot, const T* prototype) {
    T* p = static_cast<T*>(slot->Get());
    if (p == InvalidPtr())
      p = NULL;
    return p;
  }

  static T* GetWithAutoCreate(ThreadLocalSlot* slot, const T* prototype) {
    T* p = static_cast<T*>(slot->Get());
    if (!p) {
      p = new T();
      slot->Set(p);
    } else if (p == InvalidPtr()) {
      p = NULL;
    }
    return p;
  }

  static void Delete(void* p) {
    T* q = static_cast<T*>(p);
    if (q != InvalidPtr())
      Deleter::Delete(q);
  }

 private:
  mutable details::ThreadLocalSlot m_slot;
  T* m_prototype;
  T* (*m_getter)(ThreadLocalSlot* slot, const T* prototype);
};

} // namespace gdt

#endif // COMMON_SYSTEM_CONCURRENCY_THREAD_LOCAL_H
