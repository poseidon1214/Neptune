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

gdt::Callback<void()>* g_cb_se = NULL;
static bool g_stop = false;
static void Handler(int signo) {
  LOG(ERROR) << "Receive signal: " << signo << ", bye";
  g_stop = true;
}

static void OnElection(gdt::ZkLeaderSelection* ls, int rc);
static void Call(gdt::ZkLeaderSelection* ls);

static void OnSessionExpired(gdt::ZkLeaderSelection* ls) {
  LOG(ERROR) << "OnSessionExpired";
  Call(ls);
}

static void OnElection(gdt::ZkLeaderSelection* ls, int rc) {
  if (rc == ZOK) {
    LOG(ERROR) << "Proposals succeed";
  } else {
    LOG(ERROR) << "Proposals fail: " << zerror(rc);
    delete g_cb_se;
    g_cb_se = gdt::NewCallback(OnSessionExpired, ls);
    gdt::ThisThread::Sleep(3000);
    Call(ls);
  }
}

static void Call(gdt::ZkLeaderSelection* ls) {
  while (!g_stop) {
    gdt::Callback<void(int rc)>* cb_election;
    cb_election = gdt::NewCallback(OnElection, ls);
    g_cb_se = gdt::NewCallback(OnSessionExpired, ls);
    int ret = ls->Proposals(cb_election, g_cb_se);
    LOG(ERROR) << "Proposals return: " << zerror(ret);
    if (ret == ZOK) {
      break;
    }
    delete cb_election;
    delete g_cb_se;
    gdt::ThisThread::Sleep(3000);
  }
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

  Call(ls);

  while (!g_stop) {
    gdt::ThisThread::Sleep(3000);
  }

  return 0;
}
