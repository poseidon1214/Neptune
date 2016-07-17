// Copyright (C), 1998-2015, Tencent
// Author: Li, Jiliang neoli@tencent.com
// Date: 2015年11月25日

#include "common/encoding/pb_json_test.pb.h"
#include "common/encoding/protobuf_utility.h"
#include "thirdparty/gtest/gtest.h"

using pb_json_test::TestMessage;

TEST(ProtobufUtility, CreateGzipSerializeAndParse) {
  TestMessage message1;
  message1.set_qua(123);
  message1.set_id(456);
  message1.set_flag(true);
  message1.set_b("foo");
  message1.mutable_nested_msg()->set_id(789);
  std::string output;
  CHECK(gdt::SerializeToGzippedString(message1, &output));
  TestMessage* message2 = dynamic_cast<TestMessage*>(
      gdt::CreateProtoMessageByName("pb_json_test.TestMessage"));
  ASSERT_TRUE(message2);
  ASSERT_TRUE(gdt::ParseFromGzippedString(output, message2));
  EXPECT_EQ(message1.qua(), message2->qua());
  EXPECT_EQ(message1.id(), message2->id());
  EXPECT_EQ(message1.flag(), message2->flag());
  EXPECT_EQ(message1.b(), message2->b());
  EXPECT_EQ(message1.nested_msg().id(), message2->nested_msg().id());
  delete message2;
}

TEST(ProtobufUtility, CreateProtoMessageByName) {
  ASSERT_FALSE(gdt::CreateProtoMessageByName("NotExistMessageName"));
}

TEST(ProtobufUtility, ParseFromGzippedString) {
  TestMessage message;
  ASSERT_FALSE(gdt::ParseFromGzippedString("InvalidInput", &message));
}
