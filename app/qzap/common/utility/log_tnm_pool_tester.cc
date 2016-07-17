// Copyright (C), 1998-2013, Tencent
// Author: svdxu@tencent.com
// Date: 2013-10-23
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

  std::map<std::string, int> key_to_attr_id;
  key_to_attr_id["NOC"] = 636371;
  key_to_attr_id["CFS"] = 636372;
  InitTNMLogSinkPool(key_to_attr_id);

  LOG(ERROR) << FLAGS_test;
  LOG_TO_SINK(TNMSink("CFS"), ERROR) << "hello cfs";
  LOG_TO_SINK(TNMSink("NOC"), ERROR) << "hello noc";
  return 0;
}
