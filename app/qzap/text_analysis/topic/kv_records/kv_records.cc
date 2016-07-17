// Copyright (c) 2013 Tencent Inc.
// Author: Wang Yi (yiwang@tencent.com)
//         Xuemin Zhao (xueminzhao@tencent.com)

#include "app/qzap/text_analysis/topic/kv_records/kv_records.h"

#include "app/qzap/common/base/scoped_ptr.h"
#include "thirdparty/glog/logging.h"

namespace qzap {
namespace text_analysis {
namespace base {

KVRecordReader::KVRecordReader(std::ifstream* fin) : fin_(fin) {}

KVRecordReader::~KVRecordReader() { fin_ = NULL; }

int32_t KVRecordReader::Read(std::string* key, std::string* value) {
  int32_t size = 0;
  fin_->read(reinterpret_cast<char*>(&size), sizeof(size));
  if (fin_->eof()) {
    return -1;
  }

  scoped_array<char> buf(new char[size]);
  if (size != 0) {
    fin_->read(buf.get(), size);
    if (fin_->eof()) {
      LOG(ERROR) << "Error occurred while reading key: "
          << "expected bytes " << size;
      return -1;
    }
  }

  key->assign(buf.get(), size);
  buf.reset(NULL);

  fin_->read(reinterpret_cast<char*>(&size), sizeof(size));
  if (fin_->eof()) {
    LOG(ERROR) << "Error occurred while reading value size: "
        << "expected bytes " << sizeof(size);
    return -1;
  }

  buf.reset(new char[size]);
  if (size > 0) {
    fin_->read(buf.get(), size);
    if (!fin_) {
      LOG(ERROR) << "Error occurred while reading value size: "
          << "expected bytes " << sizeof(size);
      return -1;
    }
  }

  value->assign(buf.get(), size);
  buf.reset(NULL);
  return 0;
}

KVRecordWriter::KVRecordWriter(std::ofstream* fout) : fout_(fout) {}

KVRecordWriter::~KVRecordWriter() { fout_ = NULL; }

int32_t KVRecordWriter::Write(const std::string& key,
                              const std::string& value) {
  int32_t size = static_cast<int32_t>(key.size());
  fout_->write(reinterpret_cast<const char*>(&size), sizeof(size));
  if (!fout_) {
    LOG(ERROR) << "Failed writing key size";
    return -1;
  }

  fout_->write(key.c_str(), size);
   if (!fout_) {
    LOG(ERROR) << "Failed writing key";
    return -1;
  }

  size = static_cast<int32_t>(value.size());
  fout_->write(reinterpret_cast<const char*>(&size), sizeof(size));
  if (!fout_) {
    LOG(ERROR) << "Failed writing value size.";
    return -1;
  }

  fout_->write(value.c_str(), size);
  if (!fout_) {
    LOG(ERROR) << "Failed writing value";
    return -1;
  }
  return 0;
}

int32_t KVRecordWriter::Flush() {
  fout_->flush();
  return 0;
}

}  // namespace base
}  // namespace text_analysis
}  // namespace qzap

