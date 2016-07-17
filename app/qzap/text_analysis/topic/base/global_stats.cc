// Copyright (c) 2011 Tencent Inc.
// Author: Yi Wang (yiwang@tencent.com)
//         Xuemin Zhao (xueminzhao@tencent.com)

#include "app/qzap/text_analysis/topic/base/global_stats.h"

#include <fstream>
#include <iostream>

#include "app/qzap/common/base/scoped_ptr.h"
#include "app/qzap/text_analysis/topic/kv_records/kv_records.h"

namespace qzap {
namespace text_analysis {
namespace base {

using std::string;

// Load global stats from a KV-record file on local filesystem.
bool GlobalStats::Load(std::ifstream* fin) {
  KVRecordReader r(fin);
  string key, value;
  if (r.Read(&key, &value) != 0) {
    return false;
  }

  DenseTopicHistogramPB pb;
  if (!pb.ParseFromString(value)) {
    LOG(ERROR) << "Message::ParseFromString failed";
    return false;
  }
  global_topic_histogram_.ParseFrom(pb);
  return true;
}

bool GlobalStats::Save(std::ofstream* fout) const {
  KVRecordWriter w(fout);
  string key(" "), value;
  DenseTopicHistogramPB pb;
  global_topic_histogram_.SerializeTo(&pb);
  pb.SerializeToString(&value);
  if (w.Write(key, value) != 0) {
    return false;
  }
  return true;
}

// Output gobal_topic_histograms into human readable format.
void GlobalStats::AppendAsString(std::string* s) const {
  global_topic_histogram_.AppendAsString(s);
  s->append("\n");
}

}  // namespace base
}  // namespace text_analysis
}  // namespace qzap
