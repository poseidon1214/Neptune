// Copyright (C), 1998-2012, Tencent
// Author: jefftang@tencent.com
// Date: 2013-3-5
#ifndef APP_QZAP_COMMON_UTILITY_PODHASH_H_
#define APP_QZAP_COMMON_UTILITY_PODHASH_H_
// a pod hash is a hash table the contains plain of data.
// The KeyType and ValueType must be POD(plain of data) type.
#include <math.h>
#include <vector>
#include <tr1/type_traits>
#include "app/qzap/common/base/base.h"
#include "app/qzap/common/base/string_utility.h"
#include "app/qzap/common/utility/hash.h"
#include "app/qzap/common/utility/intrusive_list.h"
template <typename KeyType, typename ValueType>
class PodHash {
  public:
   // gcc 4.1 did not support __is_pod
   // COMPILE_ASSERT(std::tr1::is_pod<KeyType>::value, KeyTypeIsNotPod);
   // COMPILE_ASSERT(std::tr1::is_pod<ValueType>::value, ValueTypeIsNotPod);
   struct DataBlock {
     intrusive_link<DataBlock> data_block_link;
     KeyType key;
     ValueType value;
   };

   typedef intrusive_list<DataBlock, &DataBlock::data_block_link> DataBlockList;
   typedef DataBlockList Bucket;
   struct Header {
     uint64_t version;
     uint64_t buffer_size;
     uint64_t bucket_size;
     uint64_t data_size;
     uint64_t key_size;
     uint64_t value_size;
     uint64_t data_block_offset;
     DataBlockList free_data_list;
   };

   PodHash() : buffer_(NULL), header_(NULL), bucket_(NULL) {
   }

   const Header *header() const {
     return header_;
   }
 
   uint64_t FreeItemNumber() const {
     const Header *h = header();
     if (h == NULL) {
       return 0;
     }
     uint64_t cnt = 0;
     for (typename DataBlockList::const_iterator i = h->free_data_list.begin();
       i != h->free_data_list.end(); ++i) {
       ++cnt;
     }
     return cnt;
   }

   int GetKeyList(std::vector<KeyType> &key_list, uint32_t max_num)
   {
       if (max_num == 0) { return 0; }
       key_list.clear();

       for (int i = 0; i < header_->bucket_size; i ++) {
           const Bucket *bucket = &bucket_[i];
           for (typename Bucket::const_iterator entry_iter = bucket->begin(); entry_iter != bucket->end(); ++ entry_iter) {
               if (!entry_iter.valid()) {
                   continue;
               }
               const DataBlock *entry = entry_iter.node();
               if (!is_valid_data_entry(entry)) {
                   continue;
               }
               key_list.push_back( *(KeyType *)(&entry->key) );
               if (key_list.size() >= max_num) {
                   return 0;
               }
           }
       }  

       return 0;
   }

   void Init(
     char *buffer, size_t buffer_size, size_t bucket_size) {
     header_ = reinterpret_cast<Header*>(buffer);
     header_->version = kPodHashVersion;
     header_->buffer_size = buffer_size;
     header_->key_size = sizeof(KeyType);
     header_->value_size = sizeof(ValueType);
     header_->bucket_size = GetMask(bucket_size) + 1;
     bucket_ = reinterpret_cast<Bucket*>(buffer + sizeof(Header));
     const uint64_t data_offset = header_->bucket_size * sizeof(Bucket) + sizeof(Header);
     const uint64_t data_size = buffer_size - data_offset;
     const uint64_t data_number = data_size / sizeof(DataBlock);
     header_->data_block_offset = data_offset;
     header_->data_size = data_number;
     char *data = buffer + data_offset;
     for (size_t i = 0; i < header_->bucket_size; ++i) {
       bucket_[i].clear();
     }
     header_->free_data_list.clear();
     for (size_t i = 0; i < data_number; ++i) {
       DataBlock *data_block = reinterpret_cast<DataBlock*>(data);
       data_block->data_block_link.reset();
       header_->free_data_list.push_back(data_block);
       data += sizeof(DataBlock);
     }
     entry_start_ = buffer + header_->data_block_offset;
     entry_end_ = entry_start_ + sizeof(DataBlock) * header_->data_size;
     buffer_ = buffer;
     bucket_mask_ = header_->bucket_size - 1;
   }

   bool Mount(char *buffer, size_t buffer_size) {
     header_ = reinterpret_cast<Header*>(buffer);
     if (header_->buffer_size > buffer_size) {
       return false;
     }
     bucket_ = reinterpret_cast<Bucket*>(buffer + sizeof(Header));
     buffer_ = buffer;
     entry_start_ = buffer + header_->data_block_offset;
     entry_end_ = entry_start_ + sizeof(DataBlock) * header_->data_size;
     bucket_mask_ = header_->bucket_size - 1;
     return true;
   }

   bool Find(const KeyType &key, ValueType *value) const {
     const uint64_t finger = hash_string(reinterpret_cast<const char*>(&key), sizeof(key));
     const int index = finger & bucket_mask_;
     const Bucket *bucket = &bucket_[index];
     for (typename Bucket::const_iterator i = bucket->begin(); i != bucket->end(); ++i) {
       if (!i.valid()) {
         return false;
       }
       const DataBlock *entry = i.node();
       if (!is_valid_data_entry(entry)) {
         return false;
       }
       int ret = memcmp(&entry->key, &key, sizeof(entry->key));
       if (ret == 0) {
         memcpy(value, &entry->value, sizeof(*value));
         return true;
       }
     }
     return false;
   }

   bool IsFull() const {
     return header_ && header_->free_data_list.empty();
   }
   bool Add(const KeyType &key, const ValueType &value, bool overwrite) {
     const uint64_t finger = hash_string(reinterpret_cast<const char*>(&key), sizeof(key));
     const int index = finger & bucket_mask_;
     Bucket *bucket = &bucket_[index];
     for (typename Bucket::iterator i = bucket->begin(); i != bucket->end(); ++i) {
       if (!i.valid()) {
         return false;
       }
       DataBlock *entry = i.node();
       if (!is_valid_data_entry(entry)) {
         return false;
       }
       if (memcmp(&entry->key, &key, sizeof(key)) == 0) {
         if (!overwrite) {
           return false;
         }
         memcpy(&entry->value, &value, sizeof(entry->value));
         return true;
       }
     }
     if (header_->free_data_list.empty()) {
       return false;
     }
     DataBlock *new_data_block = &*header_->free_data_list.begin();
     header_->free_data_list.erase(new_data_block);
     new_data_block->data_block_link.reset();
     memcpy(&new_data_block->key, &key, sizeof(new_data_block->key));
     memcpy(&new_data_block->value, &value, sizeof(new_data_block->value));
     bucket->push_front(new_data_block);
     return true;
   }

   bool Delete(const KeyType &key) {
     const uint64_t finger = hash_string(reinterpret_cast<const char*>(&key), sizeof(key));
     const int index = finger & bucket_mask_;
     Bucket *bucket = &bucket_[index];
     for (typename Bucket::iterator i = bucket->begin(); i != bucket->end(); ++i) {
       if (!i.valid()) {
         return false;
       }
       DataBlock *entry = i.node();
       if (!is_valid_data_entry(entry)) {
         return false;
       }
       if (memcmp(&entry->key, &key, sizeof(key)) == 0) {
         bucket->erase(entry);
         header_->free_data_list.push_back(entry);
         return true;
       }
     }
     return false;
   }
  private:
   static const int kPodHashVersion = 1;
   int GetMask(int size) {
     int log2_size = static_cast<int>(log(size > 0 ? size : 1) /
       log(2.0));
     return (2 << log2_size) - 1;
   }

   bool is_valid_data_entry(const DataBlock *data)  const {
     const char *p = reinterpret_cast<const char*>(data);
     if (p < entry_start_ || (p + sizeof(data)) >= entry_end_) {
       return false;
     }
     const size_t offset = p - entry_start_;
     return offset / sizeof(DataBlock) * sizeof(DataBlock) == offset;
   }
   char *buffer_;
   Header *header_;
   Bucket *bucket_;
   const char *entry_start_, *entry_end_;
   uint64_t bucket_mask_;
};
#endif  // APP_QZAP_COMMON_UTILITY_PODHASH_H_
