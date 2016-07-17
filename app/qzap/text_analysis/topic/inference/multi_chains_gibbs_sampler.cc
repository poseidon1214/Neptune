// Copyright 2013 Tencent Inc.
// Author: Lifeng Wang (fandywang@tencent.com)

#include "app/qzap/text_analysis/topic/inference/multi_chains_gibbs_sampler.h"

#include <algorithm>
#include <string>
#include <tr1/unordered_map>
#include <vector>

#include "app/qzap/text_analysis/topic/base/common.h"
#include "app/qzap/text_analysis/topic/base/document.h"
#include "app/qzap/text_analysis/topic/base/model.h"
#include "app/qzap/text_analysis/topic/base/random.h"

namespace qzap {
namespace text_analysis {
namespace base {

void MultiChainsGibbsSampler::Interpret(
    const std::vector<std::string>& doc_tokens,
    std::vector<std::pair<int32_t, double> >* topic_dist) const {
  MTRandom random;
  random.SeedRNG(GenerateRandomSeed(doc_tokens));
  DoubleMatrix word_topic_dists_cache;

  std::tr1::unordered_map<int32_t, double> accumulated_topic_dist;
  std::vector<std::pair<int32_t, double> > tmp_topic_dist;
  for (int32_t i = 0; i < num_markov_chains_; ++i) {
    InterpretOneChain(doc_tokens, &tmp_topic_dist,
                      &word_topic_dists_cache, &random);

    for (size_t j = 0; j < tmp_topic_dist.size(); ++j) {
      accumulated_topic_dist[tmp_topic_dist[j].first] +=
          tmp_topic_dist[j].second;
    }
  }

  topic_dist->resize(accumulated_topic_dist.size());
  int32_t i = 0;
  std::tr1::unordered_map<int32_t, double>::const_iterator citer;
  for (citer = accumulated_topic_dist.begin();
       citer != accumulated_topic_dist.end();
       ++citer) {
    (*topic_dist)[i++] = std::make_pair(citer->first,
                                        citer->second / num_markov_chains_);
  }
  std::sort(topic_dist->begin(), topic_dist->end(), CompareByProb);
}

}  // namespace base
}  // namespace text_analysis
}  // namespace qzap

