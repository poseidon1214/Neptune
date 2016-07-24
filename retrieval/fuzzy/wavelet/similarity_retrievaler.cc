// Copyright (c) 2014, Tencent Inc.
// Author: Wang Qian<cernwang@tencent.com>

#include <math.h>
#include <map>
#include <vector>
#include <algorithm>
#include <utility>
#include <string>
#include <sys/stat.h>

#include "thirdparty/gflags/gflags.h"
#include "thirdparty/glog/logging.h"
#include "app/qzap/common/base/scoped_ptr.h"
#include "app/qzap/common/base/string_utility.h"
#include "app/qzap/common/base/callback.h"
#include "app/qzap/common/base/scoped_ptr.h"
#include "app/qzap/common/base/sysinfo.h"
#include "app/qzap/common/base/walltime.h"
#include "app/qzap/common/base/string_utility.h"
#include "app/qzap/common/thread/threadpool.h"
#include "app/qzap/common/thread/mutex.h"
#include "app/qzap/common/sstable/sstable.h"
#include "app/qzap/common/utility/file_utility.h"
#include "app/qzap/common/utility/time_utility.h"
#include "data_collector/feeder/candidate/similarity/similarity_retrievaler.h"

DECLARE_int32(top_k_num);
DECLARE_double(thres);
DEFINE_int32(retrieval_thread_num, System_NumCPUs() - 1 , "检索线程数");

namespace gdt {
namespace dynamic_creative {

static std::vector<string> Split(const string& src, const string& separator) {
  std::vector<string> dest;
  std::string str = src;
  std::string substring;
  std::string::size_type start = 0, index;

  do {
    index = str.find_first_of(separator, start);

    if (index != string::npos) {
      substring = str.substr(start, index - start);
      dest.push_back(substring);
      start = str.find_first_not_of(separator, index);

      if (start == string::npos) return dest;
    }
  } while (index != string::npos);

  substring = str.substr(start);
  dest.push_back(substring);
  return dest;
}

bool CmpSimilarity(const std::pair<int64_t, double>& left,
                   const std::pair<int64_t, double>& right) {
  return left.second > right.second;
}

double SimilarityRetrievaler::CosineSimilarity(
  std::vector<IdWeightPair>* left,
  std::vector<IdWeightPair>* right) {
  double cosine_similarity = 0.0;
  double left_norm = 0.0;
  double right_norm = 0.0;
  double cross_product = 0.0;

  if ((*left).size() == 0 || (*right).size() == 0) {
    return 0;
  }
  std::sort((*left).begin(), (*left).end());
  std::sort((*right).begin(), (*right).end());
  size_t i = 0, j = 0;
  while (i < (*left).size() && j < (*right).size()) {
    if ((*left)[i].first < (*right)[j].first) {
      left_norm += ((*left)[i].second * (*left)[i].second);
      i++;
    } else if ((*left)[i].first > (*right)[j].first) {
      right_norm += ((*right)[j].second * (*right)[j].second);
      j++;
    } else {
      cross_product += ((*left)[i].second * (*right)[j].second);
      left_norm += ((*left)[i].second * (*left)[i].second);
      right_norm += ((*right)[j].second * (*right)[j].second);
      i++;
      j++;
    }
  }
  while (i < (*left).size()) {
    left_norm += ((*left)[i].second * (*left)[i].second);
    i++;
  }
  while (j < (*right).size()) {
    right_norm += ((*right)[j].second * (*right)[j].second);
    j++;
  }
  if (left_norm > 0 && right_norm > 0) {
    cosine_similarity = cross_product / sqrt(left_norm * right_norm);
  }

  return cosine_similarity;
}

void SimilarityRetrievaler::Build(const std::string& filename) {
  InvertIndex invert_index;
  string line;
  std::ifstream ifs(filename.c_str());
  getline(ifs, line);
  while (!ifs.eof()) {
    vector<string> raw_doc = Split(line, "\t");

    if (raw_doc.size() < 1) {
      std::cout << "File Format Error" << std::endl;
    } else {
      uint64_t docid;
      CHECK(StringToNumeric(raw_doc[0], &docid));
      for (uint64_t i = 1; i < raw_doc.size(); i++) {
        vector<string> raw_pair = Split(raw_doc[i], ":");
        uint64_t termid;
        CHECK(StringToNumeric(raw_pair[0], &termid));
        double weight;
        CHECK(StringToNumeric(raw_pair[1], &weight));
        invert_index[termid].push_back(std::make_pair(docid, weight));
        forward_index[docid].push_back(std::make_pair(termid, weight));
      }
    }
    getline(ifs, line);
  }
  BuildWavletTree(invert_index);
}

void SimilarityRetrievaler::BuildWithLabels(const std::string& filename) {
  InvertIndex invert_index;
  LabelInvertIndex label_invert_index;
  string line;
  std::ifstream ifs(filename.c_str());
  while (getline(ifs, line)) {
    vector<string> raw_doc = Split(line, "\t");
    if (raw_doc.size() < 1) {
      std::cout << "File Format Error" << std::endl;
    } else {
      uint64_t docid;
      CHECK(StringToNumeric(raw_doc[0].substr(0, raw_doc[0].find("<")),
                            &docid));
      std::string label_str = raw_doc[0].substr(raw_doc[0].find("<") + 1,
                                                raw_doc[0].size() - raw_doc[0].find("<") - 2);
      vector<string> raw_labels = Split(label_str, ",");
      for (uint64_t i = 0; i < raw_labels.size(); i++) {
        uint64_t labelid;
        CHECK(StringToNumeric(raw_labels[i], &labelid));
        label_invert_index[labelid].push_back(docid);
        label_forward_index[docid].push_back(labelid);
      }
      for (uint64_t i = 1; i < raw_doc.size(); i++) {
        vector<string> raw_pair = Split(raw_doc[i], ":");
        uint64_t termid;
        CHECK(StringToNumeric(raw_pair[0], &termid));
        double weight;
        CHECK(StringToNumeric(raw_pair[1], &weight));
        invert_index[termid].push_back(std::make_pair(docid, weight));
        forward_index[docid].push_back(std::make_pair(termid, weight));
      }
    }
  }
  BuildWavletTreeWithLabels(invert_index, label_invert_index);
}

void SimilarityRetrievaler::SplitDocuments(
  const std::vector<Document>& docs,
  uint32_t split_num,
  std::vector<std::vector<Document> >* docs_shards) {
  for (size_t i = 0; i < split_num; i++) {
    (*docs_shards)[i].reserve(docs.size() / split_num + 1);
  }
  for (size_t i = 0; i < docs.size(); i++) {
    (*docs_shards)[i % split_num].push_back(docs[i]);
  }
}

void SimilarityRetrievaler::RetrievalWithInputFile(
  const std::string& input,
  const std::string& output) {
  std::vector<Document> docs;
  std::string line;
  std::ifstream ifs(input.c_str());
  std::ofstream ofs(output.c_str());
  while (getline(ifs, line)) {
    vector<string> raw_doc = Split(line, "\t");
    Document document;
    if (raw_doc.size() < 1) {
      std::cout << "File Format Error" << std::endl;
    } else {
      CHECK(StringToNumeric(raw_doc[0].substr(0, raw_doc[0].find("<")),
                            &document.id));
      std::string label_str = raw_doc[0].substr(raw_doc[0].find("<") + 1,
                                                raw_doc[0].size() - raw_doc[0].find("<") - 2);
      vector<string> raw_labels = Split(label_str, ",");
      for (uint64_t i = 0; i < raw_labels.size(); i++) {
        uint64_t labelid;
        CHECK(StringToNumeric(raw_labels[i], &labelid));
        document.labels.push_back(labelid);
      }
      for (uint64_t i = 1; i < raw_doc.size(); i++) {
        vector<string> raw_pair = Split(raw_doc[i], ":");
        uint64_t termid;
        CHECK(StringToNumeric(raw_pair[0], &termid));
        double weight;
        CHECK(StringToNumeric(raw_pair[1], &weight));
        document.contents.push_back(std::make_pair(termid, weight));
      }
    }
    docs.push_back(document);
  }
  ifs.close();
  std::vector<std::pair<uint64_t, SimilarDocments> > results;
  LOG(INFO) << "docs" << docs.size();
  MultiThreadRetrieval(docs, &results);

  for (std::vector<std::pair<uint64_t, SimilarDocments> >::const_iterator it =
         results.begin();
       it != results.end(); it++) {
    ofs << it -> first;
    for (int i = 0; i < std::min((it->second).size(), size_t(10)); i++) {
      ofs << "\t" << (it->second)[i].first << ":"
          << (it->second)[i].second;
    }
    ofs << std::endl;
  }
  ofs.close();
}

void SimilarityRetrievaler::MultiThreadRetrieval(
  const std::vector<Document>& docs,
  std::vector<std::pair<uint64_t, SimilarDocments> >* results) {
  shared_ptr<ThreadPool> thread_executor;
  if (FLAGS_retrieval_thread_num > 1) {
    thread_executor = ThreadPool::Create("Similarity Retrieval",
                                         FLAGS_retrieval_thread_num);
    if (thread_executor.get() != NULL) {
      thread_executor->Start();
    }
  }
  std::vector<std::vector<Document> > docs_shards;
  std::vector<std::vector<std::pair<uint64_t, SimilarDocments> > > result_shards;
  docs_shards.resize(FLAGS_retrieval_thread_num);
  result_shards.resize(FLAGS_retrieval_thread_num);
  LOG(INFO) << "Retrieval thread_num:" << FLAGS_retrieval_thread_num;
  SplitDocuments(docs, FLAGS_retrieval_thread_num, &docs_shards);
  for (int i = 0; i < docs_shards.size(); ++i) {
    Closure* cb_req = ::NewCallback(this,
                                    &SimilarityRetrievaler::SingleThreadRetrieval,
                                    docs_shards[i],
                                    &result_shards[i]);
    if (thread_executor.get() != NULL) {
      thread_executor->PushTask(cb_req);
    } else {
      cb_req->Run();
    }
  }
  if (thread_executor.get() != NULL) {
    thread_executor->Stop();
  }
  for (int i = 0; i < result_shards.size(); ++i) {
    std::copy(result_shards[i].begin(), result_shards[i].end(),
              back_inserter(*results));
  }
}

void SimilarityRetrievaler::SingleThreadRetrieval(
  const std::vector<Document>& docs,
  std::vector<std::pair<uint64_t, SimilarDocments> >* results) {
  LOG(INFO) << "SingleThread" << docs.size();
  for (std::vector<Document>::const_iterator it = docs.begin();
       it != docs.end(); it++) {
    SimilarDocments result;
    RetrievalWithLabelFilterAndReranking(*it, &result);
    results->push_back(std::make_pair(it->id, result));
  }
}

void SimilarityRetrievaler::RetrievalWithLabelFilterAndReranking(
  const Document& doc,
  SimilarDocments* result) {
  SimilarDocments filter_results;
  std::vector<int64_t> raw_results;
  std::map<int64_t, int64_t> label_results_num;
  const std::vector<uint64_t>& labels = doc.labels;
  for (int j = 0; j < labels.size(); j++) {
    label_results_num[labels[j]] = 0;
  }

  RetrievalWithLabelFilter(doc.contents, labels, FLAGS_thres, &raw_results);
  for (int i = 0; i < raw_results.size(); i++) {
    filter_results.push_back(std::make_pair(raw_results[i],
                                            CosineSimilarity(const_cast<WeightedVecotr*>(&doc.contents),
                                                             &forward_index[raw_results[i]])));
  }
  std::sort(filter_results.begin(), filter_results.end(), CmpSimilarity);
  // 去除之前的逻辑
/* 
  for (int i = 0; i < std::min(filter_results.size(), size_t(FLAGS_top_k_num)); i++) {
    result->push_back(filter_results[i]);
  }
*/
  // 对所有标签出满top k的量
  for (int i = 0; i < filter_results.size(); i++) {
    bool need = false;
    for (uint64_t j = 0; j <
         label_forward_index[filter_results[i].first].size(); j++) {
      if (label_results_num.find(
            label_forward_index[filter_results[i].first][j]) !=
          label_results_num.end() &&
          label_results_num[label_forward_index[filter_results[i].first][j]]
          < FLAGS_top_k_num) {
        need = true;
        label_results_num[label_forward_index[filter_results[i].first][j]]++;
        break;
      }
    }
    if (need) {
      result->push_back(filter_results[i]);
    }
  }
}

void SimilarityRetrievaler::DocmentsToTopKList(const std::string& input,
                                               const std::string& output,
                                               double FLAGS_thres) {
  InvertIndex invert_index;
  string line;
  std::ifstream ifs(input.c_str());
  std::ofstream ofs(output.c_str());
  while (getline(ifs, line)) {
    vector<string> raw_doc = Split(line, "\t");
    if (raw_doc.size() < 1) {
      std::cout << "File Format Error" << std::endl;
    } else {
      uint64_t docid;
      CHECK(StringToNumeric(raw_doc[0], &docid));
      for (uint64_t i = 1; i < raw_doc.size(); i++) {
        vector<string> raw_pair = Split(raw_doc[i], ":");
        uint64_t termid;
        CHECK(StringToNumeric(raw_pair[0], &termid));
        double weight;
        CHECK(StringToNumeric(raw_pair[1], &weight));
        invert_index[termid].push_back(std::make_pair(docid, weight));
        forward_index[docid].push_back(std::make_pair(termid, weight));
      }
    }
  }
  BuildWavletTree(invert_index);
  std::vector<std::pair<int64_t, double> > filter_results;

  for (ForwardIndex::const_iterator it = forward_index.begin();
       it != forward_index.end(); it++) {
    ofs << it -> first;
    RetrievalWithReranking(it -> second, FLAGS_thres, it -> first,
                           &filter_results);
    for (int i = 0; i < std::min(filter_results.size(), size_t(10)); i++) {
      ofs << "\t" << filter_results[i].first << ":"
          << filter_results[i].second;
    }
    filter_results.clear();
    ofs << std::endl;
  }
  ifs.close();
  ofs.close();
}

void SimilarityRetrievaler::DocmentsToTopKListWithLabels(
  const std::string& input,
  const std::string& output,
  double thres) {
  InvertIndex invert_index;
  LabelInvertIndex label_invert_index;
  string line;
  std::ifstream ifs(input.c_str());
  std::ofstream ofs(output.c_str());
  while (getline(ifs, line)) {
    vector<string> raw_doc = Split(line, "\t");
    if (raw_doc.size() < 1) {
      std::cout << "File Format Error" << std::endl;
    } else {
      uint64_t docid;
      CHECK(StringToNumeric(raw_doc[0].substr(0, raw_doc[0].find("<")),
                            &docid));
      std::string label_str = raw_doc[0].substr(raw_doc[0].find("<") + 1,
                                                raw_doc[0].size() - raw_doc[0].find("<") - 2);
      vector<string> raw_labels = Split(label_str, ",");
      for (uint64_t i = 0; i < raw_labels.size(); i++) {
        uint64_t labelid;
        CHECK(StringToNumeric(raw_labels[i], &labelid));
        label_invert_index[labelid].push_back(docid);
        label_forward_index[docid].push_back(labelid);
      }
      for (uint64_t i = 1; i < raw_doc.size(); i++) {
        vector<string> raw_pair = Split(raw_doc[i], ":");
        uint64_t termid;
        CHECK(StringToNumeric(raw_pair[0], &termid));
        double weight;
        CHECK(StringToNumeric(raw_pair[1], &weight));
        invert_index[termid].push_back(std::make_pair(docid, weight));
        forward_index[docid].push_back(std::make_pair(termid, weight));
      }
    }
  }
  BuildWavletTreeWithLabels(invert_index, label_invert_index);
  std::vector<std::pair<int64_t, double> > filter_results;
  std::vector<int64_t> raw_results;
  for (ForwardIndex::const_iterator it = forward_index.begin();
       it != forward_index.end(); it++) {
    // 每个label对应的结果数
    std::map<int64_t, int64_t> label_results_num;
    ofs << it -> first;
    std::vector<uint64_t>& labels = label_forward_index[it -> first];
    for (int j = 0; j < labels.size(); j++) {
      label_results_num[labels[j]] = 0;
    }
    RetrievalWithLabelFilter(it -> second, labels, FLAGS_thres, &raw_results);
    for (int i = 0; i < raw_results.size(); i++) {
      // if (raw_results[i] != it -> first) {
      filter_results.push_back(std::make_pair(raw_results[i],
                                              CosineSimilarity(&forward_index[it -> first],
                                                               &forward_index[raw_results[i]])));
      // }
    }
    std::sort(filter_results.begin(), filter_results.end(), CmpSimilarity);
    for (int i = 0; i < filter_results.size(); i++) {
      bool need = false;
      for (uint64_t j = 0; j <
           label_forward_index[filter_results[i].first].size(); j++) {
        if (label_results_num.find(
              label_forward_index[filter_results[i].first][j]) !=
            label_results_num.end() &&
            label_results_num[label_forward_index[filter_results[i].first][j]]
            < FLAGS_top_k_num) {
          need = true;
          label_results_num[label_forward_index[filter_results[i].first][j]]++;
          break;
        }
      }
      if (need) {
        ofs << "\t" << filter_results[i].first << ":"
            << filter_results[i].second;
      }
    }
    filter_results.clear();
    raw_results.clear();
    ofs << std::endl;
  }
  ifs.close();
  ofs.close();
}

void SimilarityRetrievaler::Retrieval(const WeightedVecotr& document,
                                      double thres,
                                      std::vector<int64_t>* results) {
  std::vector<std::pair<std::pair<size_t, size_t>, double> > patterns;
  for (int i = 0; i < document.size(); i++) {
    if (feature_offset.find(document[i].first) != feature_offset.end()) {
      patterns.push_back(std::make_pair(feature_offset[document[i].first],
                                        document[i].second * document[i].second));
    }
  }
  wavelet_tree.Retrieval(patterns, thres , results);
}

void SimilarityRetrievaler::RetrievalWithLabelFilter(
  const WeightedVecotr& document,
  const std::vector<uint64_t>& labels,
  double thres,
  std::vector<int64_t>* results) {
  std::vector<std::pair<std::pair<size_t, size_t>, double> > patterns;
  std::vector<std::pair<size_t, size_t> > filter_patterns;
  for (int i = 0; i < document.size(); i++) {
    patterns.push_back(std::make_pair(std::make_pair(
                                      feature_offset[document[i].first].first,
                                      feature_offset[document[i].first].second),
                                      document[i].second * document[i].second));
  }
  for (int i = 0; i < labels.size(); i++) {
    filter_patterns.push_back(label_offset[labels[i]]);
  }
  wavelet_tree.RetrievalWithFilter(patterns, filter_patterns, thres , results);
}

void SimilarityRetrievaler::RetrievalWithReranking(
  WeightedVecotr* document,
  double thres,
  std::vector<std::pair<int64_t, double> >* filter_results,
  int64_t* retrieval_time,
  int64_t* reanking_time) {
  std::vector<int64_t> raw_results;
  int64_t begin_time = GetCurrentTimeMillis();
  Retrieval(*document, thres, &raw_results);
  int64_t end_time1 = GetCurrentTimeMillis();
  if (retrieval_time != NULL) {
    (*retrieval_time) = end_time1 - begin_time;
  }
  for (int i = 0; i < raw_results.size(); i++) {
    (*filter_results).push_back(std::make_pair(raw_results[i],
                                               CosineSimilarity(document, &forward_index[raw_results[i]])));
  }
  std::sort((*filter_results).begin(), (*filter_results).end(), CmpSimilarity);
  int64_t end_time2 = GetCurrentTimeMillis();
  if (reanking_time != NULL) {
    (*reanking_time) = end_time2 - end_time1;
  }
}

void SimilarityRetrievaler::RetrievalWithReranking(
  const WeightedVecotr& document, double thres, int64_t docid,
  std::vector<std::pair<int64_t, double> >* filter_results) {
  std::vector<int64_t> raw_results;
  for (int i = 0; i < raw_results.size(); i++) {
    if (raw_results[i] != docid) {
      (*filter_results).push_back(std::make_pair(raw_results[i],
                                                 CosineSimilarity(&forward_index[docid],
                                                                  &forward_index[raw_results[i]])));
    }
  }
  std::sort((*filter_results).begin(), (*filter_results).end(), CmpSimilarity);
}

void SimilarityRetrievaler::Intersect(
  const vector<uint64_t>& termids, size_t thres,
  std::vector<int64_t>* results) {
  std::vector<std::pair<size_t, size_t> > patterns;

  for (int i = 0; i < termids.size(); i++) {
    patterns.push_back(feature_offset[termids[i]]);
  }
  wavelet_tree.Intersect(patterns, thres , 10, results);
}

// TODO(cernwang)
void SimilarityRetrievaler::ForwardIndexToInvertedIndex(
  const ForwardIndex& forward_index,
  InvertIndex* invert_index) {
}

void SimilarityRetrievaler::BuildWavletTree(const InvertIndex& invert_index) {
  std::vector<uint64_t> posting;

  for (InvertIndex::const_iterator it = invert_index.begin();
       it != invert_index.end(); it++) {
    feature_offset[it -> first].first = posting.size();

    for (std::vector<std::pair<uint64_t, double> >::const_iterator iter =
           (it -> second).begin(); iter != (it -> second).end(); iter++) {
      posting.push_back(iter -> first);
    }
    feature_offset[it -> first].second = posting.size();
  }

  wavelet_tree.Init(posting);
}

void SimilarityRetrievaler::BuildWavletTreeWithLabels(
  const InvertIndex& invert_index,
  const LabelInvertIndex label_invert_index) {
  std::vector<uint64_t> posting;
  for (InvertIndex::const_iterator it = invert_index.begin();
       it != invert_index.end(); it++) {
    feature_offset[it -> first].first = posting.size();
    for (std::vector<std::pair<uint64_t, double> >::const_iterator iter =
           (it -> second).begin(); iter != (it -> second).end(); iter++) {
      posting.push_back(iter -> first);
    }
    feature_offset[it -> first].second = posting.size();
  }
  for (LabelInvertIndex::const_iterator it = label_invert_index.begin();
       it != label_invert_index.end(); it++) {
    label_offset[it -> first].first = posting.size();
    for (std::vector<uint64_t>::const_iterator iter = (it -> second).begin();
         iter != (it -> second).end(); iter++) {
      posting.push_back(*iter);
    }
    label_offset[it -> first].second = posting.size();
  }
  wavelet_tree.Init(posting);
}

}  // namespace dynamic_creative
}  // namespace gdt
