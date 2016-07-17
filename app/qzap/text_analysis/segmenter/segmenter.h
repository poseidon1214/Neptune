// Copyright (c) 2011 Tencent Inc.
// Author: Huan Yu (huanyu@tencent.com)

#ifndef APP_QZAP_TEXT_ANALYSIS_SEGMENTER_SEGMENTER_H_
#define APP_QZAP_TEXT_ANALYSIS_SEGMENTER_SEGMENTER_H_

#include <string>
#include <vector>

#include "common/system/concurrency/thread_local.h"
#include "thirdparty/gflags/gflags.h"
#include "thirdparty/tcwordseg/TCSegFunc.h"
#include "common/base/singleton.h"

DECLARE_string(segmenter_data_dir);

namespace qzap {
namespace text_analysis {

class SegmenterInitializer {
 public:
  SegmenterInitializer();
  ~SegmenterInitializer();
};

typedef gdt::Singleton<SegmenterInitializer> SingletonSegmenterInitializer;

class SegmenterHandler {
 public:
  SegmenterHandler() : handle_(NULL) {};
  bool Init();
  tcwordseg::HANDLE GetHandle() { return handle_; }
  ~SegmenterHandler();

 private:
  tcwordseg::HANDLE handle_;
  static const int kDefaultTCWordSegFlag =
      (TC_T2S|TC_RUL|TC_U2L| TC_USR|TC_CN|TC_ENGU| TC_S2D|TC_POS);
};

// This is a C++ style wrapper of tcwordseg, all text_analysis components should
// call this interface instead of using tcwordseg directly.
//
// NOTE(huanyu): We use ThreadLocalPtr internally for segment handler, so it
// is threadsafe.
//
// See unittest for usage examples.
class Segmenter {
 public:
  Segmenter() {};
  ~Segmenter() {};

  bool Init();

  bool Segment(const std::string& text, std::vector<std::string>* tokens) const;
  bool SegmentWithWordType(const std::string& text,
                           std::vector<std::string>* tokens,
                           std::vector<std::string>* word_types) const;
 private:
  mutable gdt::ThreadLocalPtr<SegmenterHandler> handler_;
};

}  // namespace text_analysis
}  // namespace qzap

#endif  // APP_QZAP_TEXT_ANALYSIS_SEGMENTER_SEGMENTER_H_
