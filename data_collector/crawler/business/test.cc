// Copyright (c) 2011, Tencent Inc.
// All rights reserved.
//
// Author: DongPing HUANG <dphuang@tencent.com>
// Created: 11/05/11
// Description:

#include "common/net/http/http_downloader.h"
#include <string>
#include "common/base/string/concat.h"
#include "thirdparty/gtest/gtest.h"
#include "thirdparty/glog/logging.h"

namespace gdt {

TEST(HttpDownloaderTest, GetWithBadURI) {
  HttpDownloader downloader;
  HttpResponse response;
  HttpDownloader::ErrorType error;
  auto url = std::string("http://www.qq.com");
  EXPECT_TRUE(downloader.Get(url, &response, &error));
}


}  // namespace gdt

