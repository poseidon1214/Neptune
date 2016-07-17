// Copyright (c) 2015, Tencent Inc.
// Author: cernwang <cernwang@tencent.com>

#include "data_collector/feeder/get_data/get_data_processor.h"

#include "thirdparty/glog/logging.h"
#include "thirdparty/gflags/gflags.h"
#include "common/base/shared_ptr.h"
#include "data_collector/feeder/get_data/index_info_load_functor.h"
#include "data_collector/feeder/get_data/raw_data_download_functor.h"

namespace gdt {

bool GetDataProcessor::Init() {
  INIT_FUNCTOR(index_info_load_functor, IndexInfoLoadFunctor);
  INIT_FUNCTOR(raw_data_download_functor, RawDataDownloadFunctor);
  success_init_ = true;
  return true;
}

}  // namespace gdt
