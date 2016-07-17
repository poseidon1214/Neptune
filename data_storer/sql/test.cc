// Copyright (c) 2014, Tencent Inc.
// Author: <cernwang@tencent.com>

#include "thirdparty/glog/logging.h"
#include "thirdparty/gflags/gflags.h"
#include "data_storer/sql/mysql_handler.h"
#include "data_collector/proto/product.pb.h"

using namespace gdt;  // NOLINT(build/namespaces)

int main(int argc, char** argv) {
  ::google::ParseCommandLineFlags(&argc, &argv, true);
  ::google::InitGoogleLogging(argv[0]);
  MysqlHandler mysql_handler;
  mysql_handler.BuildTable<Product>();
  return 0;
}
