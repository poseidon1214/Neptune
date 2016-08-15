// Copyright (c) 2015, Tencent Inc.
// Author: cernwang <cernwang@tencent.com>

#include "data_collector/feeder/get_data/raw_data_download_functor.h"
#include "common/net/http/http_downloader.h"

#include <time.h>


namespace gdt {
namespace wavelet {

bool RawDataDownloadFunctor::Init() {
  downloader_.Init();
  return true;
}

FunctorResult RawDataDownloadFunctor::DoWork(DataMessage* data_message) {
  std::for_each(data_message->feeder_files.begin(),
                data_message->feeder_files.end(),
                [=](FeederFile& feeder_file) {
                  if (!NeedDownload(feeder_file)) {
                    feeder_file.set_downloaded(true);
                    return;
                  }
                  if (!downloader_.DoDownloadFile(feeder_file.url(), feeder_file.filename())) {
                    feeder_file.set_downloaded(false);
                    remove(feeder_file.filename().c_str());
                  } else {
                    feeder_file.set_downloaded(true);
                  }
                });
  return kSuccess;
}

// TODO(cernwang) 现在是全量更新
bool RawDataDownloadFunctor::NeedDownload(const FeederFile& feeder_file) {
  return true;
}

}  // namespace wavelet
}  // namespace gdt
