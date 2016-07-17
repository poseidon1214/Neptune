// Copyright (c) 2011 Tencent Inc.
// Author: Xuemin Zhao (xueminzhao@tencent.com)

#include "app/qzap/text_analysis/classifier/time.h"

namespace qzap {
namespace text_analysis {

static Time gs_t_;

void Tic() { gs_t_.Tic(); }

double Toc() { return gs_t_.Toc(); }

}  // namespace text_analysis
}  // namespace qzap

