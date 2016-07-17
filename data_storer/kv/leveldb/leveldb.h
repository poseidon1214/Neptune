// Copyright (c) 2014, Tencent Inc.
// Author: Wang Qian<cernwang@tencent.com>

#ifndef CREATIVE_DYNAMIC_CREATIVE_COMMON_LEVELDB_H_
#define CREATIVE_DYNAMIC_CREATIVE_COMMON_LEVELDB_H_


#include <string>
#include <list>
#include "thirdparty/glog/logging.h"
#include "thirdparty/leveldb/db.h"
#include "thirdparty/leveldb/comparator.h"
#include "app/qzap/common/base/shared_ptr.h"
#include "app/qzap/common/utility/file_utility.h"

namespace gdt {
namespace dynamic_creative {

// 对leveldb的一个封装
class NaiveComparator : public ::leveldb::Comparator {
 public:
  // Three-way comparison function:
  // if a < b: negative result
  // if a > b: positive result
  // else: zero result
  int Compare(const ::leveldb::Slice& a, const ::leveldb::Slice& b) const {
    std::string str_a(a.data(), a.size());
    std::string str_b(b.data(), b.size());
    if (str_a > str_b) {
      return 1;
    } else if (str_a < str_b) {
      return -1;
    }
    return 0;
  }
  
  // Ignore the following methods for now:
  const char* Name() const {
    return "NaiveComparator";
  }

  void FindShortestSeparator(std::string*, const ::leveldb::Slice&) const {}
  void FindShortSuccessor(std::string*) const {}
};


template<typename Comp>
class Table {
 public:
  typedef std::string key_type;
  typedef std::string value_type;
  typedef int size_type;
  typedef shared_ptr< ::leveldb::Iterator> CursorType;
  typedef ::leveldb::DB*  raw_type_ptr;

  explicit Table(const std::string& file = "")
    : db_(NULL), comp_(), file_(file) {
  }

  ~Table() {
    Close();
  }

  bool Open(const std::string& file) {
    file_ = file;
    return Open();
  }

  bool Open() {
    //  close first if opened
    Close();
    ::leveldb::Options options;
    options.create_if_missing = true;
    options.comparator = &comp_;
    ::leveldb::Status status = ::leveldb::DB::Open(options, file_, &db_);
    if (status.ok())
      return true;

    return false;
  }
  std::string GetFileName() const {
    return file_;
  }
  void Close() {
    if (db_) {
      CheckHandle_(db_);
      delete db_;
      db_ = NULL;
    }
  }

  bool IsOpened() const {
    return CheckHandle_(db_);
  }

  bool Flush() {
    // reopen to flush cached data.
    return CheckHandle_(db_);
  }

  size_type Size() const {
    // Not supported now, use iterator
    size_type size = 0;
    if (CheckHandle_(db_)) {
      ::leveldb::ReadOptions options;
      options.fill_cache = false;
      ::leveldb::Iterator* it = db_->NewIterator(options);
      for (it->SeekToFirst(); it->Valid(); it->Next()) {
        ++size;
      }
      delete it;
    }
    return size;
  }
  /**
   * @deprecated
   */
  size_type NumItems() const {
    return Size();
  }
  bool Empty() const {
    return Size() == 0;
  }

  /*
   * @attention:
   * clear method will destroy the leveldb Instance,
   * be sure all DB iterators have been destroyed
   * before calling this method
   */
  bool Clear() {
    if (IsOpened()) {
      Close();
      std::string cmd = "rm -rf " + file_;
      system(cmd.c_str());
      if (FileExisting(file_.c_str())) {
         LOG(ERROR) << "remove  " << file_ << " failed !" << std::endl;
      }
      Open();
    }
    return true;
  }

  /**
   * @brief Insert new data into database.
   *
   * If a record with the same key exists in the database, this function has no effect.
   *
   * @param key Key std::string
   * @param value Value std::string
   *
   * @return If successful, the return value is @c true, else, it is @c false.
   */
  bool Insert(const std::string& key, const std::string& value) {
    return CheckHandle_(db_) && (db_->Put(
        ::leveldb::WriteOptions(),
        ::leveldb::Slice(key.data(), key.size()),
        ::leveldb::Slice(value.data(), value.size())).ok());
  }

  /**
   * @brief Insert new data into database or update the existing record.
   *
   * @param key Key std::string
   * @param value Value std::string
   *
   * @return If successful, the return value is @c true, else, it is @c false.
   */
  bool Update(const std::string& key, const std::string& value) {
    return Insert(key, value);
  }
  /**
   * @brief Insert new data into database or update the existing record.
   *
   * @param data data record
   *
   * @return If successful, the return value is @c true, else, it is @c false.
   */

  bool Append(const std::string& key, const std::string& value) {
    /// not supported yet
    return false;
  }
  bool Get(const std::string& key, std::string* value) const {
    if (CheckHandle_(db_)) {
      ::leveldb::Status s = db_->Get(::leveldb::ReadOptions(),
          ::leveldb::Slice(key.data(), key.size()), value);
      if (s.ok()) {
        return true;
      }
    }
    return false;
  }

  bool Del(const std::string& key) {
    return CheckHandle_(db_) &&
           db_->Delete(
             ::leveldb::WriteOptions(),
             ::leveldb::Slice(key.data(), key.size())).ok();
  }

  CursorType Begin() const {
    if (IsOpened()) {
      ::leveldb::ReadOptions options;
      options.fill_cache = false;
      CursorType cursor(db_->NewIterator(options));
      cursor->SeekToFirst();
      return cursor;
    }
    return CursorType();
  }

  CursorType Begin(const std::string& key) const {
    if (IsOpened()) {
      ::leveldb::ReadOptions options;
      options.fill_cache = false;
      CursorType cursor(db_->NewIterator(options));
      cursor->Seek(::leveldb::Slice(key.data(), key.size()));
      return cursor;
    }
    return CursorType();
  }

  CursorType RBegin() const {
    if (IsOpened()) {
      ::leveldb::ReadOptions options;
      options.fill_cache = false;
      CursorType cursor(db_->NewIterator(options));
      cursor->SeekToLast();
      return cursor;
    }
    return CursorType();
  }

  bool Fetch(CursorType* cursor, std::string* key, std::string* value) {
    if (IsOpened() && (*cursor).get() && (*cursor)->Valid()) {
      (*key) = std::string(const_cast<char*>((*cursor)->key().data()),
                           static_cast<std::size_t>((*cursor)->key().size()));
      (*value) = std::string(const_cast<char*>((*cursor)->value().data()),
          static_cast<std::size_t>((*cursor)->value().size()));

      return true;
    }
    return false;
  }

  bool IterNext(CursorType* cursor) {
    if (IsOpened() && (*cursor).get() && (*cursor)->Valid()) {
      (*cursor)->Next();
      return true;
    } else {
      (*cursor).reset();
      return false;
    }
  }

  bool IterPrev(CursorType* cursor) {
    if (IsOpened() && (*cursor).get() && (*cursor)->Valid()) {
      (*cursor)->Prev();
      return true;
    } else {
      (*cursor).reset();
      return false;
    }
  }

 private:
  static bool CheckHandle_(::leveldb::DB* h) {
    return h;
  }

  ::leveldb::DB* db_;
  Comp comp_;
  std::string file_;
};

}  // namespace dynamic_creative
}  // namespace gdt

#endif  // CREATIVE_DYNAMIC_CREATIVE_COMMON_LEVELDB_H_

