// Copyright (c) 2011 Tencent Inc.
// Author: Yi Wang (yiwang@tencent.com)
//         Xuemin Zhao (xueminzhao@tencent.com)

#include "app/qzap/text_analysis/topic/base/word_stats.h"

#include <fstream>

#include "app/qzap/common/base/scoped_ptr.h"
#include "app/qzap/common/base/string_utility.h"
#include "app/qzap/text_analysis/topic/kv_records/kv_records.h"

namespace qzap {
namespace text_analysis {
namespace base {

using std::string;
using std::tr1::unordered_map;

bool WordStats::Load(std::ifstream* fin, bool clear_old) {
  if (clear_old) {
    Clear();
  }

  KVRecordReader r(fin);
  bool ok = true;
  int32_t ret = 0;
  string key, value;
  int32_t word_id;
  SparseTopicHistogramPB pb;
  while ((ret = r.Read(&key, &value)) == 0) {
    if (!StringToNumeric(key, &word_id)) {
      LOG(ERROR) << "failed to read word_id: " << key;
      ok = false;
      break;
    }

    if (word_topic_histograms_.find(word_id) !=
        word_topic_histograms_.end()) {
      LOG(ERROR) << "word id " << word_id << " repeated";
      ok = false;
      break;
    }

    pb.ParseFromString(value);

    word_topic_histograms_[word_id] = new WordTopicHistogram(NumTopics());
    word_topic_histograms_[word_id]->ParseFrom(pb, NumTopics());
  }

  return ok;
}

bool WordStats::LoadAndMerge(const std::vector<std::string>& filenames) {
  Clear();
  for (size_t i = 0; i < filenames.size(); ++i) {
    LOG(INFO) << "Loading word stats from " << filenames[i];
    std::ifstream fin(filenames[i].c_str());
    if (fin.fail() || !Load(&fin, false)) {
      LOG(ERROR) << "failed to load word stats from " << filenames[i];
      return false;
    }
    fin.close();
  }
  return true;
}

bool WordStats::Save(std::ofstream* fout) const {
  KVRecordWriter w(fout);
  string key, value;
  SparseTopicHistogramPB sparse_pb;
  for (ConstIterator it(this); !it.Done(); it.Next()) {
    key = ConvertToString(it.Word());
    it.GetTopicHistogram().SerializeTo(&sparse_pb);
    sparse_pb.SerializeToString(&value);
    if (w.Write(key, value) != 0) {
      return false;
    }
  }
  return true;
}

void WordStats::ParseFrom(const unordered_map<int32_t,
                          DenseTopicHistogram>& word_stats) {
  Clear();
  typedef unordered_map<int32_t, DenseTopicHistogram>::const_iterator Iter;
  for (Iter cit = word_stats.begin(); cit != word_stats.end(); ++cit) {
    CHECK(NumTopics() == cit->second.NumTopics());
    word_topic_histograms_[cit->first] = new WordTopicHistogram(NumTopics());
    word_topic_histograms_[cit->first]->ParseFrom(cit->second);
  }
}

int32_t WordStats::NumSparseNodes() const {
  int32_t count = 0;
  std::tr1::unordered_map<int32_t, WordTopicHistogram*>::const_iterator it =
      word_topic_histograms_.begin();
  while (it != word_topic_histograms_.end()) {
    count += it->second->Length();
    ++it;
  }
  return count;
}

double WordStats::SparseRatio() const {
  if (NumTopics() == 0) {
    LOG(ERROR) << "NumTopics = 0";
    return 0;
  }
  if (NumWords() == 0) {
    LOG(ERROR) << "NumWords = 0";
    return 0;
  }
  return static_cast<double>(NumSparseNodes()) / NumWords() / NumTopics();
}

void WordStats::AppendAsString(std::string* s, const Vocabulary* v) const {
  for (ConstIterator it(this); !it.Done(); it.Next()) {
    it.AppendAsString(s, v);
  }
}

void WordStats::Clear() {
  std::tr1::unordered_map<int32_t, WordTopicHistogram*>::iterator
      histogram_iterator_;
  for (histogram_iterator_ = word_topic_histograms_.begin();
       histogram_iterator_ != word_topic_histograms_.end();
       ++histogram_iterator_) {
    delete histogram_iterator_->second;
  }
  word_topic_histograms_.clear();
}

}  // namespace base
}  // namespace text_analysis
}  // namespace qzap
