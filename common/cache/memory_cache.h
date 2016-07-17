// Copyright (c) 2011, Tencent Inc.
// All rights reserved.
//
// Author: TaoCheng <timcheng@tencent.com>
// A LRU-Based unthread-safe Memory cache

#ifndef COMMON_CACHE_MEMORY_CACHE_H_
#define COMMON_CACHE_MEMORY_CACHE_H_

#include <stdio.h>
#include <sys/time.h>

#include <map>

#include "common/base/container_of.h"
#include "common/base/stdext/intrusive_list.h"
#include "common/system/time/gettimestamp.h"

#include "thirdparty/glog/logging.h"

namespace gdt {

enum MemoryCacheOpResult {
  NOT_IN_CACHE= 1,
  EXPIRED = 2,
  CAPACITY_NOT_ENOUGH = 3,
  NOT_EXPIRED = 4
};

template <typename Key, typename Value>
class MemoryCache {
  struct Node {
    Node() : expiry_time_in_ms(0) {}
    Value value;
    uint64_t expiry_time_in_ms;
    mutable list_node link;
  };
  typedef std::map<Key, Node> MapType;

 public:
  MemoryCache(size_t capacity, int life_cycle_in_ms)
    : capacity_(capacity), life_cycle_in_ms_(life_cycle_in_ms) {
  }

  size_t Capacity() const {
    return capacity_;
  }

  size_t Size() const {
    return map_.size();
  }

  bool IsEmpty() const {
    return map_.empty();
  }

  bool IsFull() const {
    return map_.size() == capacity_;
  }

  // insert if not exist, replace if exist
  bool Put(const Key& key, const Value& value) {
    VLOG(50) << "Entry MemoryCache Put";
    Node& node = map_[key];
    uint64_t timestamp = GetTimeStampInMs();

    if (node.expiry_time_in_ms != 0 &&
        node.expiry_time_in_ms > timestamp) {
      VLOG(50) << "Do not update it, expire time:" << node.expiry_time_in_ms
               << ",timestamp:" << timestamp;
      return false;
    }

    node.value = value;
    node.expiry_time_in_ms = timestamp + life_cycle_in_ms_;
    MarkAsHot(&node);
    if (map_.size() > capacity_)
      DiscardOneElement();
    return true;
  }

  int PutWithDetail(const Key& key, const Value& value) {
    VLOG(50) << "Entry MemoryCache PutWithDetail";
    Node& node = map_[key];
    uint64_t timestamp = GetTimeStampInMs();

    if (node.expiry_time_in_ms != 0 &&
        node.expiry_time_in_ms > timestamp) {
      VLOG(50) << "Do not update it, expire time:" << node.expiry_time_in_ms
               << ",timestamp:" << timestamp;
      return NOT_EXPIRED;
    }

    node.value = value;
    node.expiry_time_in_ms = timestamp + life_cycle_in_ms_;
    MarkAsHot(&node);
    if (map_.size() > capacity_) {
      DiscardOneElement();
      return CAPACITY_NOT_ENOUGH;
    }
    return 0;
  }

  // ret false if not found
  bool Get(const Key& key, Value* value) const {
    VLOG(50) << "Entry MemoryCache Get";
    typename MapType::const_iterator it = map_.find(key);
    if (it == map_.end())
      return false;

    if (it->second.expiry_time_in_ms < GetTimeStampInMs()) {
      VLOG(50) << "Data has been expiried";
      return false;
    }
    *value = it->second.value;
    MarkAsHot(&it->second);
    return true;
  }

  int GetWithDetail(const Key& key, Value* value) const {
    VLOG(50) << "Entry MemoryCache GetWithDetail";
    typename MapType::const_iterator it = map_.find(key);
    if (it == map_.end())
      return NOT_IN_CACHE;

    if (it->second.expiry_time_in_ms < GetTimeStampInMs()) {
      VLOG(50) << "Data has been expiried";
      return EXPIRED;
    }
    *value = it->second.value;
    MarkAsHot(&it->second);
    return 0;
  }

  // @return whether found
  bool Contains(const Key& key) const {
    typename MapType::const_iterator it = map_.find(key);
    if (it == map_.end())
      return false;
    if (it->second.expiry_time_in_ms < GetTimeStampInMs())
      return false;
    return true;
  }


 private:
  void MarkAsHot(const Node* node) const {
    if (!lru_list_.empty() && node == &lru_list_.back())
      return;
    node->link.unlink();
    lru_list_.push_back(*const_cast<Node*>(node));
  }

  bool DiscardOneElement() {
    if (!lru_list_.empty()) {
      typename intrusive_list<Node>::iterator it = lru_list_.begin();
      Node* node = &*it;
      typename MapType::value_type* pair =
        container_of(node, typename MapType::value_type, second);
      map_.erase(pair->first);
      return true;
    }
    return false;
  }

 private:
  size_t capacity_;
  MapType map_;
  int life_cycle_in_ms_;
  mutable intrusive_list<Node> lru_list_;
};

}  // namespace gdt

#endif  // COMMON_CACHE_MEMORY_CACHE_H_

