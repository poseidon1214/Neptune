// Copyright (C), 1998-2013, Tencent
// Author: jefftang@tencent.com
// Date: 2013-1-19
#include <vector>
#include "app/qzap/common/utility/net_utility.h"
#include "app/qzap/common/utility/spp_proto_send_and_recv.h"
#include "app/qzap/common/utility/spp_proto_send_and_recv_test.pb.h"
#include "thirdparty/gtest/gtest.h"
#include "thirdparty/glog/logging.h"

// should be the same with spp_proto_send_and_recv_test_svr.cc
static const uint8_t kSppProtoSendAndRecvTestVersion = 0xef;
static const uint8_t kSppProtoSendAndRecvTestCommand = 1;
static const char kTestInterfaceName[] = "eth1";
static const char kSppTestServerStart[] = "$PATH_QZONE_PRJ/app/qzap/common/utility/spp_proto_send_and_recv_test_data/bin/start.sh";  // NOLINT
static const char kSppTestServerStop[] = "$PATH_QZONE_PRJ/app/qzap/common/utility/spp_proto_send_and_recv_test_data/bin/stop.sh";  // NOLINT
class SppProtoSendAndRecvTest : public testing::Test {
  void SetUp() {
    setenv("PATH_QZONE_PRJ", "../../../../../..", true);
    unsetenv("HEAPCHECK");
    LOG(INFO) << "SetUp: " << kSppTestServerStart;
    system(kSppTestServerStop);
    sleep(1);
    system(kSppTestServerStop);
    sleep(1);
    system(kSppTestServerStart);
    sleep(1);
  }

  void TearDown() {
    LOG(INFO) << "TearDown: " << kSppTestServerStop;
    system(kSppTestServerStop);
  }
};

TEST_F(SppProtoSendAndRecvTest, Test1) {
  SppProtoSendAndRecv client(
    kSppProtoSendAndRecvTestVersion,
    GetIpAddressByInterfaceName(kTestInterfaceName),
    12345, 5000);
  SppProtoSendAndRecvTestResponse response;
  SppProtoSendAndRecvTestRequest request;
  std::string blob1(10<<20, 'x');
  std::string blob2(11<<20, 'x');
  request.set_a(1);
  request.set_b(1);
  request.set_blob(blob1);
  ASSERT_TRUE(client.SendAndRecv(kSppProtoSendAndRecvTestCommand, &request,
                                 &response));
  ASSERT_EQ(response.c(), request.a());
  ASSERT_EQ(response.d(), request.b());
  ASSERT_EQ(response.blob(), blob1);
  system(kSppTestServerStop);
  sleep(1);
  system(kSppTestServerStop);
  sleep(1);
  ASSERT_FALSE(client.SendAndRecv(kSppProtoSendAndRecvTestCommand, &request,
                                  &response));
  system(kSppTestServerStart);
  sleep(5);
  request.set_a(2);
  request.set_b(2);
  request.set_blob(blob2);
  ASSERT_TRUE(client.SendAndRecv(kSppProtoSendAndRecvTestCommand, &request,
                                 &response));
  ASSERT_EQ(response.c(), request.a());
  ASSERT_EQ(response.d(), request.b());
  ASSERT_EQ(response.blob(), blob2);
}
