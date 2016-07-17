// Copyright (c) 1998-2015 Tencent Technology Inc. All rights reserved.
// Author: foreveryang
// Create: Tue Dec  8 14:53:16 CST 2015

#ifndef COMMON_ZOOKEEPER_ZK_ADAPTOR_H_
#define COMMON_ZOOKEEPER_ZK_ADAPTOR_H_

#include <string>
#include <vector>
#include "common/base/callback.h"
#include "common/system/concurrency/event.h"

#include "thirdparty/gtest/gtest_prod.h"
#include "thirdparty/zookeeper/zookeeper.h"

namespace gdt {

class ZkAdaptor {
 public:
  explicit ZkAdaptor(
      const std::string& host,
      // user:passwd
      const std::string& credential = "");
  virtual ~ZkAdaptor();

  // timeout_ms: -1 wait until connected
  //              0 return immediately
  //             >0 wait until connected || timeout
  // return ZOO_ERRORS
  virtual int WaitConnected(int timeout_ms);

  typedef Callback<void(int rc, const std::string& data, int version)>
      GetCallback;
  typedef Callback<void(int rc, int version)> SetCallback;
  struct AclItem {
    std::string scheme;
    std::string id;
    int32_t perms;
    AclItem() {
    }
    AclItem(const std::string& s,
            const std::string& i,
            int32_t p)
        : scheme(s),
          id(i),
          perms(p) {
    }
    explicit AclItem(const ACL& zk_acl)
        : scheme(zk_acl.id.scheme),
          id(zk_acl.id.id),
          perms(zk_acl.perms) {
    }
  };

  // return ZOO_ERRORS
  virtual int Create(const std::string& name, const std::string& data);
  virtual int CreateUnsafe(const std::string& name, const std::string& data);
  virtual int Get(const std::string& name, std::string* data,
                  int32_t* version);
  virtual int Set(const std::string& name, const std::string& data,
                  int32_t version);
  virtual int AsyncGet(const std::string& name, GetCallback* callback);
  virtual int AsyncSet(const std::string& name, const std::string& data,
                       int32_t version, SetCallback* callback);
  virtual int Delete(const std::string& name, int32_t version);
  virtual int Exists(const std::string& name);

  virtual int GetChildren(const std::string& name,
                          std::vector<std::string>* children);

  virtual int GetAcls(const std::string& name,
                      std::vector<AclItem>* acls,
                      int32_t* acl_version);
  // if `scheme' is `digest',
  //    id is `username:(base64(sha1sum(username:password)))'
  //    use '//common/zookeeper/tool/digest_authentication_provider.sh' to
  //    generate it
  virtual int AddAcl(const std::string& name, int32_t acl_version,
                     const std::string& scheme, const std::string& id,
                     int32_t perms);
  virtual int RemoveAcl(const std::string& name, int32_t acl_version,
                        const std::string& scheme, const std::string& id,
                        size_t* removed_count);

 private:
  FRIEND_TEST(ZkAdaptorTest, GlobalWatcher);
  FRIEND_TEST(ZkAdaptorTest, AddAuth);

  virtual int AddAuth(int timeout_ms);
  int SetAcls(const std::string& name, int32_t acl_version,
              const std::vector<AclItem>& acls);
  bool CheckDigestId(const std::string& digest_id);

  static void GlobalWatcher(
      zhandle_t* zh, int type, int state, const char* path, void* ctx);
  void Close();

  std::string zk_host_;
  zhandle_t* zh_;
  std::string credential_;
  const int max_get_length_;

  AutoResetEvent connected_event_;
};
}  // namespace gdt

#endif  // COMMON_ZOOKEEPER_ZK_ADAPTOR_H_
