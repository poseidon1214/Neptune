// Copyright (c) 2013 Tencent Inc.
// Author: Wang Yi (yiwang@tencent.com)
//         Xuemin Zhao (xueminzhao@tencent.com)

#ifndef APP_QZAP_TEXT_ANALYSIS_TOPIC_KVRECORDS_KVRECORDS_H_
#define APP_QZAP_TEXT_ANALYSIS_TOPIC_KVRECORDS_KVRECORDS_H_

#include <stdint.h>
#include <fstream>

namespace qzap {
namespace text_analysis {
namespace base {

class KVRecordReader {
 public:
  explicit KVRecordReader(std::ifstream* fin);
  ~KVRecordReader();

  // return code:
  //  -1, some error occurred;
  //  0, successful;
  //  1, EOF
  int32_t Read(std::string* key, std::string* value);

 private:
  std::ifstream* fin_;
};

class KVRecordWriter {
 public:
  explicit KVRecordWriter(std::ofstream* fout);
  ~KVRecordWriter();

  int32_t Write(const std::string& key, const std::string& value);
  int32_t Flush();

 private:
  std::ofstream* fout_;
};

}  // namespace base
}  // namespace text_analysis
}  // namespace qzap

#endif  // APP_QZAP_TEXT_ANALYSIS_TOPIC_KVRECORDS_KVRECORDS_H_
