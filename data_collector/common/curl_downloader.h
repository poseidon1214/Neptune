// Copyright (c) 2014, Tencent Inc.
// Author: <cernwang@tencent.com>

#ifndef DATA_COLLECTOR_COMMON_CURL_DOWNLOADER_H_
#define DATA_COLLECTOR_COMMON_CURL_DOWNLOADER_H_

#include <string>
#include "thirdparty/curl/include/curl.h"

namespace gdt {

class CurlDownloader {
 public:
  CurlDownloader();
  virtual ~CurlDownloader();
  bool Init();
  void UnInit();
  bool DoDownload(const std::string &url, std::string *data);
  bool DoDownloadFile(const std::string &url, const std::string& path);

 private:
  CURL *curl_;
};

}  // namespace gdt
#endif  // DATA_COLLECTOR_COMMON_CURL_DOWNLOADER_H_
