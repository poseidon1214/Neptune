// Copyright (C), 1998-2013, Tencent
// Author: jefftang@tencent.com
// Date: 2013-4-17
#include <sys/types.h>
#include <sys/stat.h>
#include "app/qzap/common/base/base.h"
#include "app/qzap/common/utility/log_tnm_utility.h"
#include "thirdparty/gflags/gflags.h"
#include "thirdparty/glog/logging.h"
DEFINE_string(test, "hello world", "the send tnm string");
DEFINE_int32(attr_id, 628966, "the send attr id");
int main(int argc, char **argv) {
  ::google::ParseCommandLineFlags(&argc, &argv, true);
  ::google::InitGoogleLogging(argv[0]);
  ::InitTNMLogSink(FLAGS_attr_id);
  LOG(ERROR) << FLAGS_test;
  return 0;
}
