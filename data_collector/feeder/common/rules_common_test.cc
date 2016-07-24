#include "data_collector/feeder/common/rules_common.h"

#include <string>
#include "common/base/string/concat.h"
#include "thirdparty/gtest/gtest.h"
#include "thirdparty/glog/logging.h"

namespace gdt {

TEST(RulesCommon, GetUnixTime) {
  EXPECT_EQ(rules::GetUnixTime("2016-07-16"), "20160716");
  EXPECT_EQ(rules::GetUnixTime("2016-07-16 05:21:46"), "20160716");
}


}  // namespace gdt

