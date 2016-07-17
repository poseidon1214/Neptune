// Copyright (c) 2014 Tencent Inc.
// Author: cernwang (cernwang@tencent.com)

#ifndef DATA_COLLECTOR_FEEDER_GET_DATA_RAW_DATA_DOWNLOAD_FUNCTOR_H_
#define DATA_COLLECTOR_FEEDER_GET_DATA_RAW_DATA_DOWNLOAD_FUNCTOR_H_

#include <vector>
#include <string>

#include "app/qzap/common/base/string_utility.h"
#include "app/qzap/common/utility/file_utility.h"
#include "data_collector/common/curl_downloader.h"
#include "data_collector/feeder/framework/base_functor.h"
#include "data_collector/feeder/framework/data_message.h"

namespace gdt {

class RawDataDownloadFunctor: public FeederBaseFunctor {
 public:
  RawDataDownloadFunctor() : FeederBaseFunctor() {
  }

  virtual bool Init();

  virtual ~RawDataDownloadFunctor() {}

  // 从文件里解析商品数据
  FunctorResult DoWork(DataMessage* data_message);

 private:
  // 是否需要进行下载
  bool NeedDownload(const FeederFile& feeder_file);

 public:
  // 下载器
  CurlDownloader downloader_;
};

}  // namespace gdt

#endif  // DATA_COLLECTOR_FEEDER_GET_DATA_RAW_DATA_DOWNLOAD_FUNCTOR_H_
