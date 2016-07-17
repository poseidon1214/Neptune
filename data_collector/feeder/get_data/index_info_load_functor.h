// Copyright (c) 2014 Tencent Inc.
// Author: cernwang (cernwang@tencent.com)

#ifndef DATA_COLLECTOR_FEEDER_GET_DATA_DATA_PARSE_FUNCTOR_H_
#define DATA_COLLECTOR_FEEDER_GET_DATA_DATA_PARSE_FUNCTOR_H_

#include <vector>
#include <string>

#include "app/qzap/common/base/string_utility.h"
#include "app/qzap/common/utility/file_utility.h"
#include "data_collector/common/curl_downloader.h"
#include "data_collector/feeder/framework/base_functor.h"
#include "data_collector/feeder/framework/data_message.h"

namespace gdt {

class IndexInfoLoadFunctor: public FeederBaseFunctor {
 public:
  IndexInfoLoadFunctor() : FeederBaseFunctor() {
  }

  virtual bool Init();

  virtual ~IndexInfoLoadFunctor() {}

  // 从文件里解析商品数据
  virtual FunctorResult DoWork(DataMessage* data_message);

 private:
  // 填充feeder文件信息
  bool FillFeederFile(
      const WebsiteConfig& website_config,
      std::vector<FeederFile>* feeder_files);

 public:
  // 下载器
  CurlDownloader downloader_;
};

}  // namespace gdt

#endif  // data_collector_FEEDER_GET_DATA_DATA_PARSE_FUNCTOR_H_
