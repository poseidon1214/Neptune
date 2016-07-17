// Copyright (c) 1998-2015 Tencent Technology Inc. All rights reserved.
// Author: foreveryang
// Create: Wed Dec 16 20:55:09 CST 2015

#include "common/zookeeper/zk_adaptor.h"

#include <string.h>
#include <string>
#include <vector>

#include "common/encoding/base64.h"
#include "common/base/stdext/string.h"    // string_as_array

#include "thirdparty/gflags/gflags.h"
#include "thirdparty/glog/logging.h"

DEFINE_int32(sequence_zk_init_timeout, 3000, "zk init timeout");
DEFINE_int32(zk_adaptor_max_get_length, 256, "zk max get length");

namespace gdt {

int ZkStateToReturnCode(int state) {
  if (state == ZOO_CONNECTED_STATE) return ZOK;
  else if (state == ZOO_AUTH_FAILED_STATE) return ZAUTHFAILED;
  else if (state == ZOO_CONNECTING_STATE) return ZCONNECTIONLOSS;
  else if (state == ZOO_EXPIRED_SESSION_STATE) return ZSESSIONEXPIRED;
  else
    return -1;
}

void ZkAdaptor::GlobalWatcher(
    zhandle_t* zh, int type, int state, const char* path, void* ctx) {
  ZkAdaptor* adaptor = static_cast<ZkAdaptor*>(ctx);
  if (state == ZOO_CONNECTED_STATE) {
    adaptor->connected_event_.Set();
    LOG(INFO) << "Received connected event";
    return;
  }

  LOG(WARNING) << "Received event, type: " << type
               << ", state: " << state
               << ", path: " << (path ? path : "NULL");
}

ZkAdaptor::ZkAdaptor(
    const std::string& host,
    const std::string& credential)
    : zk_host_(host), zh_(NULL), credential_(credential),
      max_get_length_(FLAGS_zk_adaptor_max_get_length) {
}

ZkAdaptor::~ZkAdaptor() {
  Close();
}

void ZkAdaptor::Close() {
  if (zh_) {
    zookeeper_close(zh_);
    zh_ = NULL;
  }
}

int ZkAdaptor::WaitConnected(int timeout_ms) {
  bool add_auth = false;
  if (zh_ == NULL) {
    zh_ = zookeeper_init(
        zk_host_.c_str(), ZkAdaptor::GlobalWatcher,
        FLAGS_sequence_zk_init_timeout, NULL, this, 0);
    if (!zh_) {
      LOG(ERROR) << "Fail to init zk handle, " << strerror(errno);
      return -1;
    }
    if (!credential_.empty()) {
      add_auth = true;
    }
  }
  int state = zoo_state(zh_);
  if (timeout_ms == 0 && state != ZOO_CONNECTED_STATE) {
    LOG(WARNING) << "Fail to wait connected state";
    return ZkStateToReturnCode(state);
  }
  if (state != ZOO_CONNECTED_STATE) {
    if (timeout_ms == -1) {
      connected_event_.Wait();
    } else {
      connected_event_.TimedWait(timeout_ms);
    }
  }
  state = zoo_state(zh_);
  if (state != ZOO_CONNECTED_STATE) {
    LOG(WARNING) << "Fail to wait connected state";
    return ZkStateToReturnCode(state);
  }

  if (add_auth) {
    return AddAuth(timeout_ms);
  }

  return ZOK;
}

int ZkAdaptor::Create(const std::string& name, const std::string& data) {
  CHECK(zh_);
  if (credential_.empty()) {
    return zoo_create(zh_, name.c_str(), data.data(), data.size(),
                      &ZOO_OPEN_ACL_UNSAFE, 0, NULL, 0);
  }
  return zoo_create(zh_, name.c_str(), data.data(), data.size(),
                    &ZOO_CREATOR_ALL_ACL, 0, NULL, 0);
}

int ZkAdaptor::CreateUnsafe(const std::string& name, const std::string& data) {
  CHECK(zh_);
  return zoo_create(zh_, name.c_str(), data.data(), data.size(),
                    &ZOO_OPEN_ACL_UNSAFE, 0, NULL, 0);
}

int ZkAdaptor::Get(const std::string& name, std::string* data,
                   int32_t* version) {
  CHECK(zh_);
  Stat stat;
  int len = max_get_length_;
  data->resize(len);
  int ret = zoo_get(zh_, name.c_str(), 0,
      string_as_array(data), &len, &stat);
  if (ret == ZOK) data->resize(len);
  if (version) {
    *version = stat.version;
  }
  return ret;
}

// no static for UT
void OnZkAsyncGetDone(int rc, const char* value, int value_len,
                      const Stat* stat, const void* user_arg) {
  typedef ZkAdaptor::GetCallback GetCallback;
  GetCallback* callback = const_cast<GetCallback*>(
      static_cast<const GetCallback*>(user_arg));
  if (rc == ZOK) {
    callback->Run(rc, std::string(value, value_len), stat->version);
  } else {
    callback->Run(rc, "", 0);
  }
}

int ZkAdaptor::AsyncGet(const std::string& name, GetCallback* callback) {
  CHECK(zh_);
  if (callback == NULL) {
    LOG(ERROR) << "Fail to check callback";
    return ZSYSTEMERROR;
  }
  return zoo_aget(zh_, name.c_str(), 0, OnZkAsyncGetDone, callback);
}

int ZkAdaptor::Set(const std::string& name,
                   const std::string& data,
                   int32_t version) {
  CHECK(zh_);
  return zoo_set(zh_, name.c_str(), data.data(), data.size(), version);
}

void OnZkAsyncSetDone(int rc, const struct Stat *stat,
                      const void* user_arg) {
  typedef ZkAdaptor::SetCallback SetCallback;
  SetCallback* callback = const_cast<SetCallback*>(
      static_cast<const SetCallback*>(user_arg));
  if (rc == ZOK) {
    callback->Run(rc, stat->version);
  } else {
    callback->Run(rc, 0);
  }
}

int ZkAdaptor::AsyncSet(const std::string& name, const std::string& data,
                        int32_t version, SetCallback* callback) {
  CHECK(zh_);
  if (callback == NULL) {
    LOG(ERROR) << "Fail to check callback";
    return ZSYSTEMERROR;
  }
  return zoo_aset(zh_, name.c_str(), data.data(), data.size(), version,
                  OnZkAsyncSetDone, callback);
}

int ZkAdaptor::Delete(const std::string& name, int32_t version) {
  CHECK(zh_);
  return zoo_delete(zh_, name.c_str(), version);
}

int ZkAdaptor::Exists(const std::string& name) {
  CHECK(zh_);
  Stat stat;
  return zoo_exists(zh_, name.c_str(), 0, &stat);
}

typedef Callback<void(int rc)> AddAuthCallback;

void PassAndSet(AutoResetEvent* event, int* user_rc, int rc) {
  *user_rc = rc;
  event->Set();
}

void OnZkAddAuthDone(int rc, const void* user_arg) {
  AddAuthCallback* callback = const_cast<AddAuthCallback*>(
      static_cast<const AddAuthCallback*>(user_arg));
  callback->Run(rc);
}

int ZkAdaptor::AddAuth(int timeout_ms) {
  CHECK(zh_);
  size_t pos = credential_.find(':');
  if (pos == std::string::npos || pos == 0 ||
      pos == credential_.size() - 1) {
    LOG(WARNING) << "Wrong credential format '" << credential_
                 << "', username:password";
    return ZAUTHFAILED;
  }
  AutoResetEvent authed_event;
  int auth_ret;
  AddAuthCallback* callback = NewCallback(PassAndSet, &authed_event, &auth_ret);
  int ret = zoo_add_auth(
      zh_, "digest", credential_.data(), credential_.size(),
      OnZkAddAuthDone, callback);
  if (ret != ZOK) {
    LOG(WARNING) << "Fail to add auth, " << zerror(ret);
    delete callback;
    return ret;
  }
  if (!authed_event.TimedWait(timeout_ms)) {
    LOG(WARNING) << "Fail to wait auth event"
                 << ", timeout: " << timeout_ms;
    Close();
    // zookeeper_close trigger the callback
    // delete callback;
    return -1;
  }
  if (auth_ret != ZOK) {
    LOG(WARNING) << "Fail to check add auth return code"
                 << ", " << zerror(auth_ret);
    return auth_ret;
  }
  return ZOK;
}

int ZkAdaptor::GetAcls(const std::string& name,
                       std::vector<AclItem>* acls,
                       int32_t* acl_version) {
  CHECK(zh_);
  ACL_vector zk_acl;
  Stat zk_stat;
  int ret = zoo_get_acl(zh_, name.c_str(), &zk_acl, &zk_stat);
  if (ret != ZOK) {
    return ret;
  }
  for (int i = 0; i < zk_acl.count; ++i) {
    acls->push_back(AclItem(zk_acl.data[i]));
  }
  if (acl_version) {
    *acl_version = zk_stat.aversion;
  }
  deallocate_ACL_vector(&zk_acl);
  return ZOK;
}

int ZkAdaptor::SetAcls(const std::string& name,
                       int32_t acl_version,
                       const std::vector<AclItem>& acls) {
  if (acls.empty()) {
    return zoo_set_acl(zh_, name.c_str(), acl_version, &ZOO_OPEN_ACL_UNSAFE);
  }
  // a small risk of stack overflow
  const int kAclSize = acls.size();
  ACL zk_acls[kAclSize];
  // no allocate
  ACL_vector acl_vector;
  acl_vector.count = kAclSize;
  acl_vector.data = zk_acls;
  for (size_t i = 0; i < kAclSize; ++i) {
    ACL& zk_acl = zk_acls[i];
    zk_acl.perms = acls[i].perms;
    zk_acl.id.scheme = const_cast<char*>(acls[i].scheme.c_str());
    zk_acl.id.id = const_cast<char*>(acls[i].id.c_str());
  }

  return zoo_set_acl(zh_, name.c_str(), acl_version, &acl_vector);
}

int ZkAdaptor::AddAcl(const std::string& name, int32_t acl_version,
                      const std::string& scheme, const std::string& id,
                      int32_t perms) {
  CHECK(zh_);
  if (scheme == "digest" && !CheckDigestId(id)) {
    LOG(ERROR) << "Fail to check digest id: " << id;
    return -1;
  }

  std::vector<AclItem> acls;
  int ret = GetAcls(name, &acls, NULL);
  if (ret != ZOK) {
    return ret;
  }

  acls.push_back(AclItem(scheme, id, perms));
  return SetAcls(name, acl_version, acls);
}

int ZkAdaptor::RemoveAcl(const std::string& name, int32_t acl_version,
                         const std::string& scheme, const std::string& id,
                         size_t* removed_count) {
  CHECK(zh_);
  std::vector<AclItem> acls;
  int ret = GetAcls(name, &acls, NULL);
  if (ret != ZOK) {
    return ret;
  }

  size_t count = 0;
  std::vector<AclItem>::iterator it = acls.begin();
  for (; it != acls.end();) {
    if (it->scheme == scheme && it->id == id) {
      it = acls.erase(it);
      count++;
    } else {
      it++;
    }
  }

  if (removed_count) {
    *removed_count = count;
  }

  return SetAcls(name, acl_version, acls);
}

bool ZkAdaptor::CheckDigestId(const std::string& digest_id) {
  // digest_id: username:base64encode
  // As sha1sum generate 160 bits result, base64code is 28 bytes and end with
  // only one '='

  if (digest_id.size() < 30) {
    LOG(ERROR) << "Fail to check digest_is size: " << digest_id.size();
    return false;
  }
  size_t pos = digest_id.find(':', 1);
  if (pos == std::string::npos || digest_id.size() - pos - 1 != 28) {
    LOG(ERROR) << "Fail to check ':' pos";
    return false;
  }
  std::string base64code(digest_id.substr(pos + 1));
  std::string sha1code;
  if (!::Base64::Decode(base64code, &sha1code)) {
    LOG(ERROR) << "Fail to encode digest_id";
    return false;
  }
  if (sha1code.size() != 20) {
    LOG(ERROR) << "Fail to check sha1 size: " << sha1code.size();
    return false;
  }
  return true;
}

int ZkAdaptor::GetChildren(const std::string& name,
                           std::vector<std::string>* children) {
  CHECK(zh_);
  String_vector strings;
  int ret = zoo_get_children(zh_, name.c_str(), 0, &strings);
  if (ret == ZOK) {
    for (int i = 0; i < strings.count; ++i) {
      children->push_back(strings.data[i]);
    }
    deallocate_String_vector(&strings);
  }
  return ret;
}

}  // namespace gdt
