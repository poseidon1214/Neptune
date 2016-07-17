// Copyright (c) 1998-2015 Tencent Technology Inc. All rights reserved.
// Author: foreveryang
// Create: Thu Dec 10 16:39:25 CST 2015

#include "common/zookeeper/zk_leader_election.h"

#include <map>
#include <string>
#include <vector>
#include "common/base/stdext/string.h"          // string_as_array
#include "common/base/string/algorithm.h"       // StringStartsWith
#include "common/base/string/string_number.h"   // StringToNumberic
#include "thirdparty/gflags/gflags.h"
#include "thirdparty/glog/logging.h"

DEFINE_int32(zk_leader_selection_timeout_ms, 3000,
    "zk_leader_selection_timeout_ms");

namespace gdt {

void ZkLeaderSelection::ZkGlobalWatcher(
    zhandle_t* zh, int type, int state, const char* path, void* ctx) {
  ZkLeaderSelection* ls = static_cast<ZkLeaderSelection*>(ctx);
  if (type == ZOO_SESSION_EVENT) {
    if (state == ZOO_CONNECTED_STATE) {
      LOG(INFO) << "Received connected event";
      ls->connected_event_.Set();
      return;
    } else {
      if (state == ZOO_CONNECTING_STATE) {
        LOG(WARNING) << "Received connecting event";
        return;
      }
    }
  }
  if (state == ZOO_EXPIRED_SESSION_STATE) {
    LOG(ERROR) << "Received expired session event, type: " << type;
    if (ls->done_) {
      ls->CallDoneRun(-1);
    } else {
      ls->CallExpiredSessionRun();
    }
    return;
  }
  LOG(WARNING) << "Received event, type: " << type
               << ", state: " << state
               << ", path: " << (path ? path : "NULL");
  return;
}

void ZkLeaderSelection::ZkNodeWatcher(
    zhandle_t* zh, int type, int state, const char* path, void* ctx) {
  ZkLeaderSelection* ls = static_cast<ZkLeaderSelection*>(ctx);
  if (type != ZOO_DELETED_EVENT) {
    LOG(WARNING) << "Received event, type: " << type
                 << ", state: " << state
                 << ", path: " << (path ? path : "NULL");
    // maybe a expired session event
    return;
  }
  ls->CheckLeader();
}

void ZkLeaderSelection::ZkMyselfWatcher(
    zhandle_t* zh, int type, int state, const char* path, void* ctx) {
  ZkLeaderSelection* ls = static_cast<ZkLeaderSelection*>(ctx);
  if (type == ZOO_DELETED_EVENT) {
    LOG(ERROR) << "Leader node has been deleted, " << ls->node_name_
               << ", path: " << path;
    ls->CallExpiredSessionRun();
    return;
  }

  LOG(WARNING) << "Received event, type: " << type
               << ", state: " << state
               << ", path: " << (path ? path : "NULL");
}

typedef Callback<void(int rc)> AddAuthedCallback;

void ZkLeaderSelection::ZkAddAuthDone(int rc, const void* data) {
  AddAuthedCallback* callback =
      const_cast<AddAuthedCallback*>(
          static_cast<const AddAuthedCallback*>(data));
  callback->Run(rc);
}

static void PassAndSet(AutoResetEvent* event, int* ret, int rc) {
  *ret = rc;
  event->Set();
}

ZkLeaderSelection::ZkLeaderSelection(
    const std::string& zk_host,
    const std::string& path,
    const std::string& credential)
    : zh_(NULL), zk_host_(zk_host), credential_(credential),
      path_(path), node_name_(path.size() + 21, '\0'),
      done_(NULL), on_expired_session_event_(NULL) {
  Reset();
}

ZkLeaderSelection::~ZkLeaderSelection() {
  Reset();
}

void ZkLeaderSelection::Reset() {
  if (done_ && !done_->IsRepeatable()) delete done_;
  done_ = NULL;
  if (on_expired_session_event_ &&
      !on_expired_session_event_->IsRepeatable()) {
    delete on_expired_session_event_;
  }
  on_expired_session_event_ = NULL;
  if (zh_) {
    int ret = zookeeper_close(zh_);
    if (ret != ZOK) {
      LOG(ERROR) << "Fail to close zk, " << zerror(ret);
    }
    zh_ = NULL;
  }
}

int ZkLeaderSelection::Proposals(
    Callback<void(int)>* done,
    Callback<void()>* on_expired_seesion_event) {
  if (zh_) {
    LOG(ERROR) << "The leader election in process";
    return -1;
  }
  // get connected zhandle
  zh_ = zookeeper_init(
      zk_host_.c_str(), ZkLeaderSelection::ZkGlobalWatcher,
      FLAGS_zk_leader_selection_timeout_ms, NULL, this, 0);
  if (zh_ == NULL) {
    LOG(ERROR) << "Fail to init zk, " << strerror(errno);
    return -1;
  }
  if (!connected_event_.TimedWait(FLAGS_zk_leader_selection_timeout_ms)) {
    LOG(ERROR) << "Fail to wait connected event, host: " << zk_host_
               << ", timeout: " << FLAGS_zk_leader_selection_timeout_ms
               << ", " << strerror(errno);
    Reset();
    return -1;
  }
  if (zoo_state(zh_) != ZOO_CONNECTED_STATE) {
    LOG(ERROR) << "Fail to connecte zk, host: " << zk_host_
               << ", state: " << zoo_state(zh_);
    Reset();
    return -1;
  }

  // add auth
  if (!credential_.empty()) {
    size_t pos = credential_.find(':');
    if (pos == std::string::npos) {
      LOG(ERROR) << "Wrong credential format '" << credential_
                 << "', {schema}:{id}";
      Reset();
      return -1;
    }
    AutoResetEvent auth_event;
    int auth_ret;
    AddAuthedCallback* cb = NewCallback(PassAndSet, &auth_event, &auth_ret);
    int ret = zoo_add_auth(
        zh_, credential_.substr(0, pos).c_str(),
        pos == (credential_.size() - 1)
            ? NULL : credential_.substr(pos + 1).data(),
        credential_.size() - pos - 1, ZkAddAuthDone, cb);
    if (ret != ZOK) {
      LOG(ERROR) << "Fail to add auth, " << zerror(ret);
      Reset();
      return ret;
    }
    if (!auth_event.TimedWait(FLAGS_zk_leader_selection_timeout_ms)) {
      LOG(ERROR) << "Fail to wait auth event"
                 << ", timeout: " << FLAGS_zk_leader_selection_timeout_ms;
      Reset();
      return -1;
    }
    if (auth_ret != ZOK) {
      LOG(ERROR) << "Fail to check add auth return code"
                 << ", " << zerror(auth_ret);
      Reset();
      return auth_ret;
    }
  }

  // prepare context
  done_ = done;
  on_expired_session_event_ = on_expired_seesion_event;
  int rc;
  AutoResetEvent event;
  if (done == NULL) {
    done_ = NewCallback(PassAndSet, &event, &rc);
  }

  CallProposals();

  if (done == NULL) {
    event.Wait();
    return rc;
  }

  return ZOK;
}

void ZkLeaderSelection::CallProposals() {
  // 1: create node
  // the zk c client api is anti-humanity
  node_name_.resize(node_name_.size() + 1);
  int ret = zoo_create(
      zh_, path_.c_str(), NULL, -1, &ZOO_READ_ACL_UNSAFE,
      ZOO_EPHEMERAL | ZOO_SEQUENCE, string_as_array(&node_name_),
      node_name_.size());
  if (ret != ZOK) {
    LOG(ERROR) << "Fail to create " << path_ << ", " << zerror(ret);
    CallDoneRun(ret);
    return;
  }
  node_name_.resize(strlen(node_name_.c_str()));

  // 2: check leader
  try_times_ = 0;
  CheckLeader();
  return;
}

void ZkLeaderSelection::CheckLeader() {
  try_times_++;
  if (try_times_ > 2) {
    LOG(WARNING) << "Check leader " << try_times_ << " times";
  }

  // path is 'ELECTION/n_'
  // node_name is 'ELECTION/n_123'
  // parent is 'ELECTION'
  // prefix is 'n_'
  // myself is 'n_123'
  size_t pos = node_name_.rfind('/');
  std::string parent = node_name_.substr(0, pos);
  std::string prefix = path_.substr(pos + 1);
  std::string myself = node_name_.substr(pos + 1);
  // hehe before return
  String_vector sv;
  int ret = zoo_get_children(zh_, parent.c_str(), 0, &sv);
  if (ret != ZOK) {
    LOG(ERROR) << "Fail to get children, " << zerror(ret);
    zoo_delete(zh_, node_name_.c_str(), -1);
    // hehe
    // deallocate_String_vector(&sv);
    CallDoneRun(ret);
    return;
  }

  uint64_t tmp;
  std::vector<std::string> children;
  for (int i = 0; i < sv.count; ++i) {
    std::string child(sv.data[i]);
    if (child.size() <= prefix.size()) continue;
    // ELECTION/josajfd
    if (!StringStartsWith(child, prefix)) continue;
    // ELECTION/n_3456
    if (child.size() != myself.size()) continue;
    // ELECTION/n_1a3
    if (!StringToNumeric(child.substr(prefix.size()), &tmp, 10)) continue;

    children.push_back(child);
  }
  // hehe
  deallocate_String_vector(&sv);

  std::string smallest;
  std::string watch;
  bool find_myself = false;
  std::vector<std::string>::iterator it = children.begin();
  for (; it != children.end(); ++it) {
    if (!find_myself && *it == myself) {
      find_myself = true;
    }
    if (smallest.empty() || *it < smallest) {
      smallest = *it;
    }
    if (*it < myself && (watch.empty() || watch < *it)) {
      watch = *it;
    }
  }
  if (!find_myself) {
    LOG(ERROR) << "I can't find myself! node_name: " << node_name_
               << ", parent: " << parent;
    CallDoneRun(-1);
    return;
  }
  if (smallest == myself) {
    ret = WatchNode(node_name_, ZkLeaderSelection::ZkMyselfWatcher);
    if (ret != ZOK) {
      zoo_delete(zh_, node_name_.c_str(), -1);
      LOG(ERROR) << "Fail to watch myself: " << node_name_;
      CallDoneRun(ret);
      return;
    }
    LOG(INFO) << "Get leader: " << node_name_
              << ", try_time: " << try_times_;
    CallDoneRun(ZOK);
    return;
  }

  LOG(INFO) << "Leader election on: " << path_
            << ", myself: " << myself
            << ", leader: " << smallest
            << ", watch: " << watch;

  // watch
  std::string watch_node(parent + "/" + watch);
  ret = WatchNode(watch_node, ZkLeaderSelection::ZkNodeWatcher);
  if (ret == ZNONODE) {
    LOG(WARNING) << "Watch node: " << watch_node << " not exist";
    CheckLeader();
    return;
  }
  if (ret != ZOK) {
    zoo_delete(zh_, node_name_.c_str(), -1);
    LOG(ERROR) << "Fail to watch:" << watch_node;
    CallDoneRun(ret);
    return;
  }
  return;
}

int ZkLeaderSelection::WatchNode(const std::string& node, watcher_fn fn) {
  int ret = zoo_wexists(zh_, node.c_str(), fn, this, NULL);
  if (ret != ZOK) {
    LOG(ERROR) << "Fail to watch node: " << node
               << ", ret: " << ret
               << "(" << zerror(ret) << ")";
  }
  return ret;
}

void ZkLeaderSelection::CallDoneRun(int rc) {
  CHECK(done_);
  Callback<void(int)>* done = done_;
  done_ = NULL;
  if (rc != ZOK) {
    // memory leak is better than coredump
    on_expired_session_event_ = NULL;
    Reset();
  }
  // if `rc' is not ZOK, application may start another election
  done->Run(rc);
  return;
}

void ZkLeaderSelection::CallExpiredSessionRun() {
  CHECK(!done_);
  CHECK(on_expired_session_event_);
  Callback<void()>* done = on_expired_session_event_;
  on_expired_session_event_ = NULL;
  Reset();
  done->Run();
}

}  // namespace gdt
