// Copyright (C), 1998-2012, Tencent
// Author: jefftang@tencent.com
// Date: 2013-2-26
#include <fcntl.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <algorithm>
#include <string>
#include <vector>
#include "app/qzap/common/base/scoped_ptr.h"
#include "app/qzap/common/base/walltime.h"
#include "app/qzap/common/base/string_utility.h"
#include "app/qzap/common/utility/net_utility.h"
#include "thirdparty/glog/logging.h"
#include "thirdparty/gtest/gtest.h"
#include "thirdparty/zlib/zlib.h"

class NetUtilityTest : public testing::Test {
 public:
  void SetUp() {
  }
};

TEST_F(NetUtilityTest, TestGetIpAddressByInterfaceName) {
  ASSERT_EQ(GetIpAddressByInterfaceName("lo"), "127.0.0.1");
}

TEST_F(NetUtilityTest, TestResolve) {
  std::vector<in_addr>  addresses;
  Resolve("0.0.0.0", &addresses);
  ASSERT_EQ(static_cast<size_t>(1), addresses.size());
  ASSERT_EQ(INADDR_ANY, addresses[0].s_addr);
}

TEST_F(NetUtilityTest, TestConnect) {
  std::vector<in_addr>  addresses;
  Resolve("0.0.0.0", &addresses);
  ASSERT_EQ(static_cast<size_t>(1), addresses.size());
  ASSERT_EQ(INADDR_ANY, addresses[0].s_addr);
  uint16_t port = PickAvailablePort();
  ASSERT_EQ(true, IsAvaliablePort(port));
  int fd = socket(AF_INET, SOCK_STREAM, 0);
  int reuse_flag = 1;
  setsockopt(fd, SOL_SOCKET, SO_REUSEADDR,
             &reuse_flag, sizeof(reuse_flag));
  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr = addresses[0];
  ASSERT_NE(-1, bind(fd, (struct sockaddr*)&addr, sizeof(addr)));
  ASSERT_EQ(0, listen(fd, 5) != 0);
  ASSERT_FALSE(IsAvaliablePort(port));
  ASSERT_NE(-1, Connect("0.0.0.0", port));
  close(fd);
}
