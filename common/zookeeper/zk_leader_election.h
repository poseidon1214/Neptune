// Copyright (c) 1998-2015 Tencent Technology Inc. All rights reserved.
// Author: foreveryang
// Create: Thu Dec 10 16:39:27 CST 2015

#ifndef COMMON_ZOOKEEPER_ZK_LEADER_ELECTION_H_
#define COMMON_ZOOKEEPER_ZK_LEADER_ELECTION_H_

#include <string>
#include "common/base/callback.h"
#include "common/system/concurrency/event.h"

#include "thirdparty/zookeeper/zookeeper.h"

namespace gdt {

class ZkLeaderSelection {
 public:
  ZkLeaderSelection(
      const std::string& zk_host,
      const std::string& path,
      const std::string& credential = "");
  virtual ~ZkLeaderSelection();

  // If `done' is NULL, this function will block until it becomes the
  // leader or it fails. You can set `done' that causes an asynchronous
  // behavior. The `rc' variable passed to the callback will tell if the
  // election succeeds(ZOK) or not(zk error).
  // If this object becomes leader, it will be owned in lifetime unless a
  // `expired session' event has been received. Accoding to zookeeper
  // documents, you should never see SESSION_EXPIRED in a correctly
  // operating cluster.
  // If ZOK return, `done' and `on_expired_session_event' will be
  // deleted(if not repeatable) when exit.
  // `on_expired_session_event' will never been called until election
  // succeed.
  virtual int Proposals(
      Callback<void(int rc)>* done,
      Callback<void()>* on_expired_session_event);

 private:
  void Reset();
  static void ZkGlobalWatcher(
      zhandle_t* zh, int type, int state, const char* path, void* ctx);
  static void ZkNodeWatcher(
      zhandle_t* zh, int type, int state, const char* path, void* ctx);
  static void ZkMyselfWatcher(
      zhandle_t* zh, int type, int state, const char* path, void* ctx);
  static void ZkAddAuthDone(int rc, const void* data);

  void CallProposals();
  void CheckLeader();
  int WatchNode(const std::string& node, watcher_fn fn);

  void CallDoneRun(int rc);
  void CallExpiredSessionRun();

  zhandle_t* zh_;
  AutoResetEvent connected_event_;

  std::string zk_host_;
  std::string credential_;
  std::string path_;
  std::string node_name_;

  Callback<void(int)>* done_;
  Callback<void()>* on_expired_session_event_;

  int try_times_;
};
}  // namespace gdt

#endif  // COMMON_ZOOKEEPER_ZK_LEADER_ELECTION_H_
