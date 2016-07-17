// Copyright (c) 1998-2016 Tencent Technology Inc. All rights reserved.
// Author: foreveryang
// Create: Wed Jan 27 11:58:20 CST 2016

#include <stdio.h>
#include <vector>
#include "common/zookeeper/zk_adaptor.h"
#include "thirdparty/gflags/gflags.h"
#include "thirdparty/glog/logging.h"
#include "thirdparty/zookeeper/zookeeper.h"

DEFINE_string(host, "127.0.0.1:2181", "host");
DEFINE_string(credential, "", "zk credentail: username:password");
DEFINE_string(cmd, "get", "get|add|remove");

DEFINE_string(name, "/XXXXXXXXX", "name");
DEFINE_int32(acl_version, -2,
    "acl version, -1 means not check version, 0 is the initialize version");

DEFINE_string(scheme, "digest", "digest|world|ip");
DEFINE_string(id, "anyone", "sub scheme");

// perms
DEFINE_bool(read, false, "perms_read");
DEFINE_bool(write, false, "perms_write");
DEFINE_bool(create, false, "perms_create");
DEFINE_bool(delete_, false, "perms_delete");
DEFINE_bool(admin, true, "perms_admin");

using gdt::ZkAdaptor;
typedef ZkAdaptor::AclItem AclItem;

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
    std::vector<AclItem> acls;
    int32_t acl_version;
    ret = zk.GetAcls(FLAGS_name, &acls, &acl_version);
    if (ret != ZOK) {
      LOG(ERROR) << "Fail to get acl, name: " << FLAGS_name
                 << ", " << zerror(ret);
      return 2;
    }
    LOG(INFO) << "Get acl succ, name: " << FLAGS_name
              << ", acl_version: " << acl_version;
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
  } else if (FLAGS_cmd == "add") {
    int perms = 0;
    if (FLAGS_read) perms |= ZOO_PERM_READ;
    if (FLAGS_write) perms |= ZOO_PERM_WRITE;
    if (FLAGS_create) perms |= ZOO_PERM_CREATE;
    if (FLAGS_delete_) perms |= ZOO_PERM_DELETE;
    if (FLAGS_admin) perms |= ZOO_PERM_ADMIN;
    ret = zk.AddAcl(FLAGS_name, FLAGS_acl_version,
                    FLAGS_scheme, FLAGS_id, perms);
    if (ret != ZOK) {
      LOG(ERROR) << "Fail to add acl, name: " << FLAGS_name
                 << ", acl_version: " << FLAGS_acl_version
                 << ", scheme: " << FLAGS_scheme
                 << ", id: " << FLAGS_id
                 << ", " << zerror(ret);
      return 3;
    }
    LOG(INFO) << "Add acl succ, name: " << FLAGS_name
               << ", scheme: " << FLAGS_scheme
               << ", id: " << FLAGS_id
               << ", R(" << bool(perms & ZOO_PERM_READ) << ")"
               << ",W(" << bool(perms & ZOO_PERM_WRITE) << ")"
               << ",C(" << bool(perms & ZOO_PERM_CREATE) << ")"
               << ",D(" << bool(perms & ZOO_PERM_DELETE) << ")"
               << ",A(" << bool(perms & ZOO_PERM_ADMIN) << ")";
    return 0;
  } else if (FLAGS_cmd == "remove") {
    size_t count;
    ret = zk.RemoveAcl(FLAGS_name, FLAGS_acl_version,
                       FLAGS_scheme, FLAGS_id, &count);
    if (ret != ZOK) {
      LOG(ERROR) << "Fail to remove acl, name: " << FLAGS_name
                 << ", acl_version: " << FLAGS_acl_version
                 << ", scheme: " << FLAGS_scheme
                 << ", id: " << FLAGS_id
                 << ", " << zerror(ret);
      return 4;
    }
    LOG(INFO) << "Remove acl succ, name: " << FLAGS_name
              << ", scheme: " << FLAGS_scheme
              << ", id: " << FLAGS_id
              << ", count: " << count;
  }
  return 0;
}
