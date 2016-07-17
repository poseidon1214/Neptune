// Copyright (c) 1998-2015 Tencent Technology Inc. All rights reserved.
// Author: foreveryang
// Create: Wed Dec 16 21:39:57 CST 2015

#include "common/zookeeper/zk_adaptor.h"

#include <string>
#include "thirdparty/gtest/gtest.h"

namespace gdt {

class ZkAdaptorTest : public ::testing::Test {
};

TEST_F(ZkAdaptorTest, WaitConnected) {
  ZkAdaptor zk("abc");
  ASSERT_EQ(-1, zk.WaitConnected(0));
  ASSERT_EQ(-1, zk.WaitConnected(1));
  ZkAdaptor zk2("127.0.0.1:121");
  ASSERT_NE(ZOK, zk2.WaitConnected(0));
  ASSERT_NE(ZOK, zk2.WaitConnected(1));
}

TEST_F(ZkAdaptorTest, Create) {
  ZkAdaptor zk("127.0.0.1:121");
  zk.WaitConnected(0);
  ASSERT_EQ(ZBADARGUMENTS, zk.CreateUnsafe("abc", "abc"));
  ASSERT_NE(ZOK, zk.CreateUnsafe("/abc", "abc"));
}

TEST_F(ZkAdaptorTest, Get) {
  ZkAdaptor zk("127.0.0.1:121");
  zk.WaitConnected(0);
  std::string data;
  ASSERT_EQ(ZBADARGUMENTS, zk.Get("abc", &data, NULL));
  ASSERT_NE(ZOK, zk.Get("/abc", &data, NULL));
}

int g_rc;
static void SetEvent(AutoResetEvent* event,
    int rc, const std::string&, int) {
  g_rc = rc;
  event->Set();
}

TEST_F(ZkAdaptorTest, AsyncGet) {
  ZkAdaptor zk("127.0.0.1:121");
  zk.WaitConnected(0);
  std::string data;
  AutoResetEvent event;
  ZkAdaptor::GetCallback* cb = NewCallback(SetEvent, &event);
  ASSERT_EQ(ZBADARGUMENTS, zk.AsyncGet("abc", cb));
  ASSERT_EQ(ZOK, zk.AsyncGet("/abc", cb));
  event.Wait();
  ASSERT_NE(ZOK, g_rc);
}

TEST_F(ZkAdaptorTest, OnZkAsyncGetDone) {
  extern void OnZkAsyncGetDone(int rc, const char* value, int value_len,
      const Stat* stat, const void* user_arg);
  AutoResetEvent event;
  ZkAdaptor::GetCallback* cb = NewCallback(SetEvent, &event);
  Stat stat;
  std::string data("abc");
  OnZkAsyncGetDone(ZOK, data.data(), data.size(), &stat, cb);
  event.Wait();
}

TEST_F(ZkAdaptorTest, Delete) {
  ZkAdaptor zk("127.0.0.1:121");
  zk.WaitConnected(0);
  ASSERT_EQ(ZBADARGUMENTS, zk.Delete("abc", 0));
  ASSERT_NE(ZOK, zk.Delete("/abc", 0));
}

TEST_F(ZkAdaptorTest, Exists) {
  ZkAdaptor zk("127.0.0.1:121");
  zk.WaitConnected(0);
  ASSERT_EQ(ZBADARGUMENTS, zk.Exists("abc"));
  ASSERT_NE(ZOK, zk.Exists("/abc"));
}

TEST_F(ZkAdaptorTest, GlobalWatcher) {
  ZkAdaptor zk("127.0.0.1:121");
  ZkAdaptor::GlobalWatcher(
      NULL, ZOO_SESSION_EVENT, ZOO_CONNECTED_STATE, "abc", &zk);
  zk.WaitConnected(-1);

  ZkAdaptor::GlobalWatcher(NULL, 1234, 0, NULL, &zk);
}

TEST_F(ZkAdaptorTest, AddAuth) {
  ZkAdaptor zk("127.0.0.1:121", "aaa");
  zk.WaitConnected(0);
  ASSERT_EQ(ZAUTHFAILED, zk.AddAuth(0));
  ZkAdaptor zk1("127.0.0.1:121", "aaa:");
  zk1.WaitConnected(0);
  ASSERT_EQ(ZAUTHFAILED, zk1.AddAuth(0));
  ZkAdaptor zk2("127.0.0.1:121", ":aaa");
  zk2.WaitConnected(0);
  ASSERT_EQ(ZAUTHFAILED, zk2.AddAuth(0));
}

}  // namespace gdt
