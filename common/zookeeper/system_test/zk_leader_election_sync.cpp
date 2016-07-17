// Copyright (c) 1998-2015 Tencent Technology Inc. All rights reserved.
// Author: foreveryang
// Create: Sat Dec 12 22:00:30 CST 2015

#include <signal.h>
#include <string>
#include "common/base/callback.h"
#include "common/system/concurrency/this_thread.h"
#include "common/zookeeper/zk_leader_election.h"
#include "thirdparty/gflags/gflags.h"
#include "thirdparty/glog/logging.h"

DEFINE_string(host, "127.0.0.1:2181", "zk_host");
DEFINE_string(credential, "", "zoo_add_auth parameter");
DEFINE_string(path, "/test_leader/n_", "prefix path");

static bool g_stop = false;
static void Handler(int signo) {
  LOG(ERROR) << "Receive signal: " << signo << ", bye";
  g_stop = true;
}

int main(int argc, char** argv) {
  google::ParseCommandLineFlags(&argc, &argv, true);
  google::InitGoogleLogging(argv[0]);

  signal(SIGINT, Handler);
  signal(SIGTERM, Handler);

  gdt::ZkLeaderSelection* ls = new gdt::ZkLeaderSelection(
      FLAGS_host, FLAGS_path, FLAGS_credential);
  if (!ls) {
    LOG(ERROR) << "Fail to get ZkLeaderSelection";
    return 1;
  }

  while (!g_stop) {
    int ret = ls->Proposals(NULL, NULL);
    if (ret == ZOK) {
      LOG(ERROR) << "Proposals succ: " << zerror(ret);
      break;
    }
    LOG(ERROR) << "Proposals fail: " << zerror(ret);
    gdt::ThisThread::Sleep(3000);
  }

  while (!g_stop) {
    gdt::ThisThread::Sleep(3000);
  }

  delete ls;
  LOG(ERROR) << "delete ZkLeaderSelection";

  return 0;
}
