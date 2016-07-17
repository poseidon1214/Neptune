// Copyright (c) 2013 Tencent Inc.
// Author: Lifeng Wang (fandywang@tencent.com)

#ifndef APP_QZAP_TEXT_ANALYSIS_TOPIC_BASE_CGO_TYPES_H_
#define APP_QZAP_TEXT_ANALYSIS_TOPIC_BASE_CGO_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef struct {
  int32_t id;  // the index of a word
  int32_t topic;  // topic assignment to current word
} Word;

typedef struct {
  void* array;
  uint64_t len;
  uint64_t cap;
} Slice;

typedef struct {
  Slice topics;
  Slice counts;
} OrderedSparseHistogram;

typedef struct {
  Slice words;
  OrderedSparseHistogram hist;
} Document;

Document* NewDocument();
void DeleteDocument(Document* doc);

Slice* NewSlice();
void DeleteWordSlice(Slice* slice);

OrderedSparseHistogram* NewOrderedSparseHistogram(uint32_t len);
void DeleteOrderedSparseHistogram(OrderedSparseHistogram* hist);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // APP_QZAP_TEXT_ANALYSIS_TOPIC_BASE_CGO_TYPES_H_

