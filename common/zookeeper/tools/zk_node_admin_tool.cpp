// Copyright (c) 1998-2016 Tencent Technology Inc. All rights reserved.
// Author: foreveryang
// Create: Thu Jan 28 16:26:43 CST 2016

#include <string>
#include <vector>
#include "common/zookeeper/zk_adaptor.h"
#include "thirdparty/gflags/gflags.h"
#include "thirdparty/glog/logging.h"
#include "thirdparty/zookeeper/zookeeper.h"

using gdt::ZkAdaptor;
typedef ZkAdaptor::AclItem AclItem;

DEFINE_string(host, "127.0.0.1:2181", "host");
DEFINE_string(credential, "", "zk credentail: username:password");
DEFINE_string(cmd, "get", "get|create|delete");

DEFINE_string(name, "/XXXXXXXXX", "name");
DEFINE_int32(data_version, -2,
    "data version, -1 means not check version, 0 is the initialize version");
DEFINE_bool(force_delete, false, "do not check node empty");

int main(int argc, char *argv[]) {
  zoo_set_log_stream(fopen("zk.log", "w"));
  FLAGS_logtostderr = true;
  google::ParseCommandLineFlags(&argc, &argv, true);
  google::InitGoogleLogging(argv[0]);

  gdt::ZkAdaptor zk(FLAGS_host, FLAGS_credential);
  int ret = zk.WaitConnected(10000);
  if (ret != ZOK) {
    LOG(ERROR) << "Fail to connect to zk, " << zerror(ret);
    return 1;
  }

  if (FLAGS_cmd == "get") {
    std::string data;
    int32_t version;
    ret = zk.Get(FLAGS_name, &data, &version);
    if (ret == ZOK) {
      std::vector<std::string> children;
      ret = zk.GetChildren(FLAGS_name, &children);
      if (ret != ZOK) {
        LOG(ERROR) << "Fail to get children, name: " << FLAGS_name
                   << ", " << zerror(ret);
        return 2;
      }
      LOG(INFO) << "Get " << FLAGS_name << " succ, version: " << version;
      LOG(INFO) << "data_size: " << data.size() <<", data: \n" << data;
      LOG(INFO) << "children:";
      for (size_t i = 0; i < children.size(); ++i) {
        LOG(INFO) << i + 1 << ": " << children[i];
      }
      return 0;
    } else if (ret == ZNONODE) {
      std::string parent;
      size_t pos = FLAGS_name.rfind('/');
      parent = FLAGS_name.substr(0, pos == 0 ? 1 : pos);
      std::vector<AclItem> acls;
      ret = zk.GetAcls(parent, &acls, NULL);
      if (ret != ZOK) {
        LOG(ERROR) << "Fail to get parent's acls, name: " << parent
                   << ", " << zerror(ret);
        return 2;
      }
      LOG(INFO) << "Node " << FLAGS_name << " not exists, parent "
                << parent << " acls:";
      for (size_t i = 0; i < acls.size(); ++i) {
        LOG(INFO) << "Auth info " << i + 1 << ": "
                  << acls[i].scheme
                  << " " << acls[i].id
                  << " R(" << bool(acls[i].perms & ZOO_PERM_READ) << ")"
                  << ",W(" << bool(acls[i].perms & ZOO_PERM_WRITE) << ")"
                  << ",C(" << bool(acls[i].perms & ZOO_PERM_CREATE) << ")"
                  << ",D(" << bool(acls[i].perms & ZOO_PERM_DELETE) << ")"
                  << ",A(" << bool(acls[i].perms & ZOO_PERM_ADMIN) << ")";
      }
      return 0;
    } else {
      LOG(ERROR) << "Fail to get node: " << FLAGS_name
                 << ", " << zerror(ret);
      return 3;
    }
  } else if (FLAGS_cmd == "create") {
    ret = zk.Create(FLAGS_name, "");
    if (ret != ZOK) {
      LOG(ERROR) << "Fail to create node " << FLAGS_name
                 << ", " << zerror(ret);
      return 4;
    }
    LOG(INFO) << "Create node " << FLAGS_name << " succ";
    return 0;
  } else if (FLAGS_cmd == "delete") {
    if (!FLAGS_force_delete) {
      std::string data;
      ret = zk.Get(FLAGS_name, &data, NULL);
      if (ret != ZOK) {
        LOG(ERROR) << "Fail to get node " << FLAGS_name
                   << ", " << zerror(ret);
        return 5;
      }
      if (!data.empty()) {
        LOG(ERROR) << "Node " << FLAGS_name << " is not empty";
        return 6;
      }
    }
    ret = zk.Delete(FLAGS_name, FLAGS_data_version);
    if (ret != ZOK) {
      LOG(ERROR) << "Fail to delete " << FLAGS_name
                 << ", version: " << FLAGS_data_version
                 << ", " << zerror(ret);
      return 7;
    }
    LOG(INFO) << "Delete " << FLAGS_name << " succ";
    return 0;
  }

  LOG(ERROR) << "Unknow cmd: " << FLAGS_cmd;

  return 8;
}
