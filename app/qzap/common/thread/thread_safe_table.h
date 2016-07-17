// Copyright    (C), 1998-2012, Tencent
// Author: jefftang@tencent.com
// Date: 2012-11-22
#ifndef APP_QZAP_COMMON_THREAD_THREAD_SAFE_TABLE_H_
#define APP_QZAP_COMMON_THREAD_THREAD_SAFE_TABLE_H_

#include <tr1/unordered_map>
#include "app/qzap/common/base/base.h"
#include "app/qzap/common/thread/shared_mutex.h"

#if !defined(_LP64)
namespace std {
namespace tr1 {
template <>
struct hash<long long unsigned int> {  // NOLINT(runtime/int)
 public:
  size_t operator()(long long unsigned int x) const {  // NOLINT(runtime/int)
    return x;
  }
};
template <>
struct hash<long long int> {  // NOLINT(runtime/int)
 public:
  size_t operator()(long long int x) const {  // NOLINT(runtime/int)
    return x;
  }
};
}  // namespace tr1
}  // namespace std
#endif

template <class Key, class Value>
class LockableTable {
 protected:
  typedef std::tr1::unordered_map<Key, Value> Table;

 public:
  typedef typename Table::const_iterator const_iterator;
  typedef typename Table::iterator iterator;
  class ScopedLock;
  class ScopedSharedLock;
  LockableTable() {
  }
  virtual ~LockableTable() {
  }

  void Lock() {
    shared_mutex_.Lock();
  }

  void Unlock() {
    shared_mutex_.Unlock();
  }

  void LockShared() {
    shared_mutex_.LockShared();
  }

  void UnlockShared() {
    shared_mutex_.UnlockShared();
  }

  bool Find(const Key &key, Value *v) {
    typename Table::const_iterator it = table_.find(key);
    if (it == table_.end()) {
      return false;
    }
    if (v != NULL) {
      *v = it->second;
    }
    return true;
  }

  bool FindAndRemove(const Key &key, Value *v) {
    typename Table::iterator it = table_.find(key);
    if (it == table_.end()) {
      return false;
    }
    if (v != NULL) {
      *v = it->second;
    }
    table_.erase(it);
    return true;
  }

  void Remove(const Key &key) {
    table_.erase(key);
  }

  void Insert(const Key &key, const Value &value) {
    table_[key] = value;
  }

  int Size() const {
    return table_.size();
  }

  void Clear() {
    table_.clear();
  }

  // The iterator is not thread safe.
  const_iterator begin() const {
    return table_.begin();
  }

  iterator begin() {
    return table_.begin();
  }

  const_iterator end() const {
    return table_.end();
  }

  iterator end() {
    return table_.end();
  }

 protected:
  SharedMutex shared_mutex_;
  Table table_;
};

template <class Key, class Value>
class LockableTable<Key, Value>::ScopedLock {
 public:
  explicit ScopedLock(LockableTable *mu)
    : mu_(mu) {
      mu_->Lock();
  }
  ~ScopedLock() {
    mu_->Unlock();
  }
 private:
  LockableTable *const mu_;
};

template <class Key, class Value>
class LockableTable<Key, Value>::ScopedSharedLock {
 public:
  explicit ScopedSharedLock(LockableTable *mu)
    : mu_(mu) {
      mu_->LockShared();
  }
  ~ScopedSharedLock() {
    mu_->UnlockShared();
  }
 private:
  LockableTable *const mu_;
};

template <class Key, class Value>
class ThreadSafeTable {
 protected:
  typedef std::tr1::unordered_map<Key, Value> Table;

 public:
  typedef typename Table::const_iterator const_iterator;
  typedef typename Table::iterator iterator;
  ThreadSafeTable() {
  }
  virtual ~ThreadSafeTable() {
  }

  bool Find(const Key &key, Value *v) {
    SharedMutex::ScopedSharedLock lock(&shared_mutex_);
    typename Table::const_iterator it = table_.find(key);
    if (it == table_.end()) {
      return false;
    }
    if (v != NULL) {
      *v = it->second;
    }
    return true;
  }

  void Remove(const Key &key) {
    SharedMutex::ScopedLock lock(&shared_mutex_);
    table_.erase(key);
  }

  bool FindAndRemove(const Key &key, Value *v) {
    SharedMutex::ScopedLock lock(&shared_mutex_);
    typename Table::iterator it = table_.find(key);
    if (it == table_.end()) {
      return false;
    }
    if (v != NULL) {
      *v = it->second;
    }
    table_.erase(it);
    return true;
  }

  void Insert(const Key &key, const Value &value) {
    SharedMutex::ScopedLock lock(&shared_mutex_);
    table_[key] = value;
  }

  int Size() const {
    return table_.size();
  }

  void Clear() {
    SharedMutex::ScopedLock lock(&shared_mutex_);
    table_.clear();
  }

  // The iterator is not thread safe.
  const_iterator begin() const {
    return table_.begin();
  }
  iterator begin() {
    return table_.begin();
  }
  const_iterator end() const {
    return table_.end();
  }
  iterator end() {
    return table_.end();
  }

 protected:
  SharedMutex shared_mutex_;
  Table table_;
};

#endif  // APP_QZAP_COMMON_THREAD_THREAD_SAFE_TABLE_H_
