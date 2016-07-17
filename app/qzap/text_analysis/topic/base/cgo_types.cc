// Copyright (c) 2013 Tencent Inc.
// Author: Lifeng Wang (fandywang@tencent.com)

#include "thirdparty/glog/logging.h"
#include "app/qzap/text_analysis/topic/base/cgo_types.h"

Document* NewDocument() {
  Document* doc = new Document;

  doc->words.array = NULL;
  doc->words.len = 0;
  doc->words.cap = 0;

  doc->hist.topics.array = NULL;
  doc->hist.counts.array = NULL;
  doc->hist.topics.len = 0;
  doc->hist.counts.len = 0;
  doc->hist.topics.cap = 0;
  doc->hist.counts.cap = 0;

  return doc;
}

void DeleteDocument(Document* doc) {
  if (doc != NULL) {
    if (doc->words.array != NULL) {
      delete [] static_cast<Word*>(doc->words.array);
      doc->words.array = NULL;
      doc->words.len = 0;
      doc->words.cap = 0;
    }
    if (doc->hist.topics.array != NULL) {
      delete [] static_cast<int32_t*>(doc->hist.topics.array);
      doc->hist.topics.array = NULL;
      doc->hist.topics.len = 0;
      doc->hist.topics.cap = 0;
    }
    if (doc->hist.counts.array != NULL) {
      delete [] static_cast<int64_t*>(doc->hist.counts.array);
      doc->hist.counts.array = NULL;
      doc->hist.counts.len = 0;
      doc->hist.counts.cap = 0;
    }
    delete doc;
    doc = NULL;
  }
}

Slice* NewSlice() {
  Slice* slice = new Slice;
  slice->array = NULL;
  slice->len = 0;
  slice->cap = 0;

  return slice;
}

void DeleteWordSlice(Slice* slice) {
  if (slice != NULL) {
    if (slice->array != NULL) {
      delete [] static_cast<Word*>(slice->array);
      slice->array = NULL;
      slice->len = 0;
      slice->cap = 0;
    }
    delete slice;
    slice = NULL;
  }
}

OrderedSparseHistogram* NewOrderedSparseHistogram(uint32_t len) {
  OrderedSparseHistogram* hist = new OrderedSparseHistogram;
  if (len > 0) {
    hist->topics.array = new int32_t[len];
    hist->counts.array = new int64_t[len];
  } else {
    hist->topics.array = NULL;
    hist->counts.array = NULL;
  }
  hist->topics.len = len;
  hist->counts.len = len;
  hist->topics.cap = len;
  hist->counts.cap = len;

  return hist;
}

void DeleteOrderedSparseHistogram(OrderedSparseHistogram* hist) {
  if (hist != NULL) {
    if (hist->topics.array != NULL) {
      delete [] static_cast<int32_t*>(hist->topics.array);
      hist->topics.array = NULL;
      hist->topics.len = 0;
      hist->topics.cap = 0;
    }
    if (hist->counts.array != NULL) {
      delete [] static_cast<int64_t*>(hist->counts.array);
      hist->counts.array = NULL;
      hist->counts.len = 0;
      hist->counts.cap = 0;
    }
    delete hist;
    hist = NULL;
  }
}
