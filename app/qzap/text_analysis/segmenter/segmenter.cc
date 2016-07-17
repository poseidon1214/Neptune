// Copyright (c) 2011 Tencent Inc.
// Author: Huan Yu (huanyu@tencent.com)
//         Lifeng Wang (fandywang@tencent.com)

#include "app/qzap/text_analysis/segmenter/segmenter.h"
#include "thirdparty/glog/logging.h"

DEFINE_string(segmenter_data_dir, "./data", "Segmenter data directory.");

using std::string;
using std::vector;

namespace tcwordseg {

/**
 * @brief 字符串自动中文分词函数
 *
 * @param hHandle   分词句柄
 * @param lpText    待切分文本
 * @param nTextLen  待切分文本长度
 * @param nCharSet  字符编码
 *
 * @return 是否切分成功
 */
bool TCSegment(tcwordseg::HANDLE hHandle,
               const char* lpText,
               size_t nTextLen = 0,
               size_t nCharSet = TC_UTF8);
}

namespace qzap {
namespace text_analysis {

SegmenterInitializer::SegmenterInitializer() {
  CHECK(tcwordseg::TCInitSeg(FLAGS_segmenter_data_dir.c_str()))
      << "Can't initialize segmenter.";
}

SegmenterInitializer::~SegmenterInitializer() {
  tcwordseg::TCUnInitSeg();
}

SegmenterHandler::~SegmenterHandler() {
  if (handle_ != NULL) {
    tcwordseg::TCCloseSegHandle(handle_);
    handle_ = NULL;
  }
}

bool SegmenterHandler::Init() {
  SegmenterInitializer& initializer = SingletonSegmenterInitializer::Instance();
  // We don't need to actually use initializer, but gcc will complain it is
  // "not used", so here it is.
  (void)initializer;

  if (handle_ == NULL) {
    handle_ = tcwordseg::TCCreateSegHandle(kDefaultTCWordSegFlag);
  }

  return handle_ != NULL;
}

bool Segmenter::Segment(const string& text, vector<string>* token) const {
  return SegmentWithWordType(text, token, NULL);
}

bool Segmenter::Init() {
  if (handler_.Get() == NULL) {
    handler_.Reset(new SegmenterHandler);
  }
  return handler_->Init();
}

bool Segmenter::SegmentWithWordType(const string& text,
                                    vector<string>* tokens,
                                    vector<string>* word_types) const {
  tokens->clear();
  if (word_types != NULL) { word_types->clear(); }
  if (handler_.Get() == NULL) { handler_.Reset(new SegmenterHandler); }
  if (!handler_->Init()) { return false; }

  if (!tcwordseg::TCSegment(handler_->GetHandle(), text.c_str())) {
    return false;
  }

  size_t words_size = tcwordseg::TCGetResultCnt(handler_->GetHandle());
  tcwordseg::pWP wordpos;
  for (size_t i = 0; i < words_size; ++i) {
    wordpos = tcwordseg::TCGetAt(handler_->GetHandle(), i);
    tokens->push_back(wordpos->word);
    if (word_types != NULL) {
      char buffer[256];
      tcwordseg::TCPosId2Str(wordpos->pos, buffer);
      word_types->push_back(buffer);
    }
  }

  return true;
}

}  // namespace text_analysis
}  // namespace qzap

