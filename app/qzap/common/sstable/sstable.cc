// Copyright (c) 2015, Tencent Inc.
// All rights reserved.

#include "thirdparty/glog/logging.h"
#include "app/qzap/common/sstable/sstable.h"
#include "app/qzap/common/utility/file_utility.h"
#include "thirdparty/leveldb/env.h"
#include "thirdparty/leveldb/table.h"
#include "thirdparty/leveldb/table_builder.h"
#include "thirdparty/libhdfs/hdfs.h"

namespace leveldb {

class MemoryRandomAccessFile : public RandomAccessFile {
 private:
    std::string str_data_;
    const char* array_data_;
    size_t array_data_size_;

 public:
    explicit MemoryRandomAccessFile(const std::string &input)
      : str_data_(input),
        array_data_(str_data_.c_str()),
        array_data_size_(str_data_.size()) {
    }

    MemoryRandomAccessFile(const char* input, size_t len)
      : array_data_(input), array_data_size_(len) { }

    virtual Status Read(uint64_t offset, size_t n, Slice* result,
                        char* scratch) const {
      Status s;
      ssize_t r = 0;
      if (offset < array_data_size_) {
        if (n + offset < array_data_size_) {
          r = n;
        } else {
          r = array_data_size_ - offset;
        }
        memcpy(scratch, array_data_ + offset, r);
      }
      *result = Slice(scratch, (r < 0) ? 0 : r);
      return s;
    }
};
}  // namespace leveldb

SSTable::SSTable() {
}

SSTable::~SSTable() {
}

SSTable *SSTable::Open(const std::string &file) {
  leveldb::RandomAccessFile *random_access_file;
  leveldb::Status status = leveldb::Env::Default()->NewRandomAccessFile(
    file, &random_access_file);
  if (!status.ok()) {
    LOG(ERROR) << "SSTable::Open " << file << " NewRandomAccessFile fail: "
      << status.ToString();
    return NULL;
  }
  leveldb::Table *table = NULL;
  leveldb::Options options;
  status = leveldb::Table::Open(
      options, random_access_file, GetFileSize(file), &table);
  if (!status.ok()) {
    LOG(ERROR) << "Table::Open fail, status: " << status.ToString();
    delete random_access_file;
    return NULL;
  }
  SSTable *sstable(new SSTable);
  sstable->table_.reset(table);
  sstable->random_access_file_.reset(random_access_file);
  sstable->filename_ = file;
  return sstable;
}

SSTable *SSTable::OpenFromString(const std::string &input) {
  leveldb::RandomAccessFile *random_access_file =
      new leveldb::MemoryRandomAccessFile(input);
  leveldb::Table *table = NULL;
  leveldb::Options options;
  leveldb::Status status =
      leveldb::Table::Open(options, random_access_file, input.size(), &table);
  if (!status.ok()) {
    LOG(ERROR) << "Table::Open fail, status: " << status.ToString();
    delete random_access_file;
    return NULL;
  }
  SSTable *sstable(new SSTable);
  sstable->table_.reset(table);
  sstable->random_access_file_.reset(random_access_file);
  sstable->filename_ = "memory string";
  return sstable;
}
SSTable *SSTable::OpenFromArray(const char *input, size_t len) {
  leveldb::RandomAccessFile *random_access_file =
      new leveldb::MemoryRandomAccessFile(input, len);
  leveldb::Table *table = NULL;
  leveldb::Options options;
  leveldb::Status status =
      leveldb::Table::Open(options, random_access_file, len, &table);
  if (!status.ok()) {
    LOG(ERROR) << "Table::Open fail, status: " << status.ToString();
    delete random_access_file;
    return NULL;
  }
  SSTable *sstable(new SSTable);
  sstable->table_.reset(table);
  sstable->random_access_file_.reset(random_access_file);
  sstable->filename_ = "memory string";
  return sstable;
}

SSTable::Iterator *SSTable::NewIterator() const {
  leveldb::Iterator *it = table_->NewIterator(leveldb::ReadOptions());
  if (!it->status().ok()) {
    LOG(ERROR) << "New leveldb iterator fail: " << it->status().ToString();
    delete it;
    return NULL;
  }
  Iterator *iterator(new Iterator);
  iterator->iterator_.reset(it);
  iterator->SeekToFirst();
  return iterator;
}

SSTable::Iterator::Iterator() {
}

SSTable::Iterator::~Iterator() {
}

bool SSTable::Iterator::Valid() const {
  return iterator_->Valid();
}

void SSTable::Iterator::SeekToFirst() {
  iterator_->SeekToFirst();
}

void SSTable::Iterator::SeekToLast() {
  iterator_->SeekToLast();
}

void SSTable::Iterator::Seek(const std::string & target) {
  iterator_->Seek(target);
}

void SSTable::Iterator::Next() {
  iterator_->Next();
}

void SSTable::Iterator::Prev() {
  iterator_->Prev();
}

std::string SSTable::Iterator::key() const {
  return std::string(iterator_->key().data(), iterator_->key().size());
}

std::string SSTable::Iterator::value() const {
  return std::string(iterator_->value().data(), iterator_->value().size());
}

std::string SSTable::Iterator::status_string() const {
  return iterator_->status().ToString();
}

SSTableBuilder::SSTableBuilder(
  const std::string &file)
  : file_(file) {
  options_.create_if_missing = true;
  options_.error_if_exists = true;
}

SSTableBuilder::SSTableBuilder(
  const std::string &file,
  const leveldb::Options &options) : file_(file), options_(options) {
}

SSTableBuilder::~SSTableBuilder() {
}


bool SSTableBuilder::Init(const leveldb::Options &options) {
  if (table_builder_.get() != NULL) {
    return true;
  }
  env_ = leveldb::Env::Default();
  if (env_->FileExists(file_)) {
    LOG(ERROR) << "SSTableBuilder::Init file exists: " << file_;
    return false;
  }
  leveldb::WritableFile *writable_file = NULL;
  leveldb::Status status = env_->NewWritableFile(
    file_, &writable_file);
  if (!status.ok()) {
    LOG(ERROR) << "NewWritableFile status is not ok: " << status.ToString();
    return false;
  }
  writable_file_.reset(writable_file);
  table_builder_.reset(
      new leveldb::TableBuilder(options, writable_file_.get()));
  if (!table_builder_->status().ok()) {
    LOG(ERROR) << "new TableBuilder fail, status: "
        << table_builder_->status().ToString();
    return false;
  }
  return true;
}

bool SSTableBuilder::Add(const std::string &key, const std::string &value) {
  if (!Init(options_)) {
    LOG(ERROR) << "new TableBuilder Init fail";
    return false;
  }
  table_builder_->Add(key, value);
  if (!table_builder_->status().ok()) {
    LOG(ERROR) << "SSTableBuilder::Add is not ok: "
      << table_builder_->status().ToString();
    return false;
  }
  return true;
}

bool SSTableBuilder::Build() {
  if (!Init(options_)) {
    LOG(ERROR) << "new TableBuilder Init fail";
    return false;
  }
  leveldb::Status status = table_builder_->Finish();
  table_builder_.reset();
  writable_file_.reset();
  if (!status.ok()) {
    LOG(ERROR) << "SSTableBuilder::Finish is not ok: " << status.ToString();
    return false;
  }
  return true;
}

uint64_t SSTableBuilder::NumEntries() const {
  if (table_builder_.get() != NULL) {
    return table_builder_->NumEntries();
  }
  return 0;
}

uint64_t SSTableBuilder::FileSize() const {
  if (table_builder_.get() != NULL) {
    return table_builder_->FileSize();
  }
  return 0;
}
