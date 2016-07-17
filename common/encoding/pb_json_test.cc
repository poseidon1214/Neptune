// Copyright (c) 2011, Tencent Inc.
// All rights reserved.
//
// Author: Eric Liu <ericliu@tencent.com>
// Created: 07/19/11
// Description: test protobuf message json serializer

#include "common/encoding/pb_json_test.pb.h"
#include <string>
#include "common/encoding/json_to_pb.h"
#include "common/encoding/pb_to_json.h"
#include "thirdparty/gtest/gtest.h"
#include "thirdparty/protobuf/descriptor.h"

namespace gdt {

using namespace std;  // NOLINT(build/namespaces)
using namespace pb_json_test;  // NOLINT(build/namespaces)
using namespace google::protobuf;  // NOLINT(build/namespaces)

class PbJsonTest : public ::testing::Test {
 public:
  virtual void SetUp();

 protected:
  string m_binary_data_b;
  string m_binary_data_bs1;
  string m_binary_data_bs2;

  string kJsonString;
  string kJsonString2;
  string kJsonString3;
  string kJsonString4;
};

void PbJsonTest::SetUp() {
  m_binary_data_b.append("你好");
  const char byte[] = { 0x00, 0x08, static_cast<char>(0xc8), 0x03, 0x12 };
  m_binary_data_b.append(byte, sizeof(byte));

  const char byte1[] = { 0x00, 0x01, 0x02, 0x03, 0x04 };
  m_binary_data_bs1.append(byte1, sizeof(byte));

  const char byte2[] = { 0x05, 0x06, 0x07, 0x08, 0x09 };
  m_binary_data_bs2.append(byte2, sizeof(byte2));

  kJsonString.assign(
    "{\n"
    "   \"b\" : \"%E4%BD%A0%E5%A5%BD%00%08%C8%03%12\",\n"
    "   \"bs\" : [ \"%00%01%02%03%04\", \"%05%06%07%08%09\" ],\n"
    "   \"flag\" : true,\n"
    "   \"id\" : 123,\n"
    "   \"long_ext\" : \"18446744073709551615\",\n"
    "   \"nested_msg\" : {\n"
    "      \"id\" : 1010,\n"
    "      \"title\" : \"test title\",\n"
    "      \"url\" : \"http://example.com/\"\n"
    "   },\n"
    "   \"nested_msgs\" : [\n"
    "      {\n"
    "         \"id\" : 456,\n"
    "         \"title\" : \"test title\",\n"
    "         \"url\" : \"http://localhost/\"\n"
    "      }\n"
    "   ],\n"
    "   \"qua\" : 456,\n"
    "   \"query\" : \"足球宝贝some\",\n"
    "   \"query_ext\" : \"含义query\",\n"
    "   \"rep_int\" : [ 1, 2 ],\n"
    "   \"test_enum\" : 2,\n"
    "   \"test_enums\" : [ 1, 2 ]\n"
    "}\n");
  kJsonString2.assign(
    "{\n"
    "   \"b\" : \"%E4%BD%A0%E5%A5%BD%00%08%C8%03%12\",\n"
    "   \"bs\" : [ \"%00%01%02%03%04\", \"%05%06%07%08%09\" ],\n"
    "   \"flag\" : true,\n"
    "   \"id\" : 123,\n"
    "   \"long_ext\" : \"18446744073709551615\",\n"
    "   \"nested_msg\" : {\n"
    "      \"id\" : 1010,\n"
    "      \"title\" : \"test title\",\n"
    "      \"url\" : \"http://example.com/\"\n"
    "   },\n"
    "   \"nested_msgs\" : [\n"
    "      {\n"
    "         \"id\" : 456,\n"
    "         \"title\" : \"test title\",\n"
    "         \"url\" : \"http://localhost/\"\n"
    "      }\n"
    "   ],\n"
    "   \"qua\" : \"456\",\n"
    "   \"query\" : \"足球宝贝some\",\n"
    "   \"query_ext\" : \"含义query\",\n"
    "   \"rep_int\" : [ 1, 2, \"3\", \"\" ],\n"
    "   \"test_enum\" : 2,\n"
    "   \"test_enums\" : [ 1, 2 ],\n"
    "   \"test_num\" : \"\"\n"
    "}\n");
  kJsonString3.assign(
    "{\n"
    "   \"b\" : \"%E4%BD%A0%E5%A5%BD%00%08%C8%03%12\",\n"
    "   \"bs\" : [ \"%00%01%02%03%04\", \"%05%06%07%08%09\" ],\n"
    "   \"flag\" : true,\n"
    "   \"id\" : 123,\n"
    "   \"long_ext\" : \"18446744073709551615\",\n"
    "   \"nested_msg\" : {\n"
    "      \"id\" : 1010,\n"
    "      \"title\" : \"test title\",\n"
    "      \"url\" : \"http://example.com/\"\n"
    "   },\n"
    "   \"nested_msgs\" : [\n"
    "      {\n"
    "         \"id\" : 456,\n"
    "         \"title\" : \"test title\",\n"
    "         \"url\" : \"http://localhost/\"\n"
    "      }\n"
    "   ],\n"
    "   \"qua\" : \"456\",\n"
    "   \"query\" : \"足球宝贝some\",\n"
    "   \"query_ext\" : \"含义query\",\n"
    "   \"rep_int\" : [ 1, 2, \"3\", \"\" ],\n"
    "   \"test_enum\" : \"V2\",\n"
    "   \"test_enums\" : [ \"V1\", \"V2\" ],\n"
    "   \"test_num\" : \"\"\n"
    "}\n");
  kJsonString4.assign(
    "{\n"
    "   \"b\" : \"%E4%BD%A0%E5%A5%BD%00%08%C8%03%12\",\n"
    "   \"bs\" : [ \"%00%01%02%03%04\", \"%05%06%07%08%09\" ],\n"
    "   \"flag\" : true,\n"
    "   \"id\" : 123,\n"
    "   \"long_ext\" : \"18446744073709551615\",\n"
    "   \"nested_msg\" : {\n"
    "      \"id\" : 1010,\n"
    "      \"title\" : \"test title\",\n"
    "      \"url\" : \"http://example.com/\"\n"
    "   },\n"
    "   \"nested_msgs\" : [\n"
    "      {\n"
    "         \"id\" : 456,\n"
    "         \"title\" : \"test title\",\n"
    "         \"url\" : \"http://localhost/\"\n"
    "      }\n"
    "   ],\n"
    "   \"qua\" : \"456\",\n"
    "   \"query\" : \"足球宝贝some\",\n"
    "   \"query_ext\" : \"含义query\",\n"
    "   \"rep_int\" : [ 1, 2, \"3\", \"\" ],\n"
    "   \"test_enum\" : \"2\",\n"
    "   \"test_enums\" : [ \"1\", \"2\" ],\n"
    "   \"test_num\" : \"\"\n"
    "}\n");
}

TEST_F(PbJsonTest, ProtoMessageToJson) {
  TestMessage test_message;
  test_message.set_id(123);
  test_message.set_qua(456);
  test_message.set_b(m_binary_data_b);
  test_message.set_query("足球宝贝some");
  test_message.set_flag(true);
  test_message.set_test_enum(TestMessage::V2);
  TestMessage::NestedMsg* msg = test_message.mutable_nested_msg();
  msg->set_id(1010);
  msg->set_title("test title");
  msg->set_url("http://example.com/");
  TestMessage::NestedMsg* msgs = test_message.add_nested_msgs();
  msgs->set_id(456);
  msgs->set_title("test title");
  msgs->set_url("http://localhost/");
  test_message.add_rep_int(1);
  test_message.add_rep_int(2);
  test_message.add_test_enums(TestMessage::V1);
  test_message.add_test_enums(TestMessage::V2);
  test_message.add_bs(m_binary_data_bs1);
  test_message.add_bs(m_binary_data_bs2);

  const Reflection* reflection = test_message.GetReflection();
  const Descriptor* descriptor = test_message.GetDescriptor();
  ASSERT_EQ(0, descriptor->extension_count());
  const FieldDescriptor* field_query_ext =
    reflection->FindKnownExtensionByName("pb_json_test.query_ext");
  ASSERT_TRUE(field_query_ext);
  reflection->SetString(&test_message, field_query_ext, "含义query");
  const FieldDescriptor* field_long_ext =
    reflection->FindKnownExtensionByName("pb_json_test.long_ext");
  ASSERT_TRUE(field_long_ext);
  reflection->SetUInt64(&test_message, field_long_ext, 0xFFFFFFFFFFFFFFFFull);

  // ASSERT_EQ(2, descriptor->extension_count());
  EXPECT_EQ("含义query",
            reflection->GetString(test_message, field_query_ext));
  EXPECT_EQ(0xFFFFFFFFFFFFFFFFull,
            reflection->GetUInt64(test_message, field_long_ext));

  string json_string;
  ASSERT_TRUE(ProtoMessageToJson(test_message, &json_string, NULL, true));
  EXPECT_EQ(kJsonString, json_string);
}

TEST_F(PbJsonTest, JsonToProtoMessage) {
  StringPiece json_data(kJsonString2);
  TestMessage test_message;
  ASSERT_TRUE(JsonToProtoMessage(json_data, &test_message));

  EXPECT_EQ(123, test_message.id());
  EXPECT_EQ(456, test_message.qua());

  EXPECT_EQ(m_binary_data_b, test_message.b());

  EXPECT_EQ("足球宝贝some", test_message.query());

  EXPECT_TRUE(test_message.flag());

  EXPECT_EQ(TestMessage::V2, test_message.test_enum());

  EXPECT_EQ(1010, test_message.nested_msg().id());
  EXPECT_EQ("test title", test_message.nested_msg().title());
  EXPECT_EQ("http://example.com/", test_message.nested_msg().url());

  ASSERT_EQ(1, test_message.nested_msgs_size());
  EXPECT_EQ(456, test_message.nested_msgs(0).id());
  EXPECT_EQ("test title", test_message.nested_msgs(0).title());
  EXPECT_EQ("http://localhost/", test_message.nested_msgs(0).url());

  ASSERT_EQ(3, test_message.rep_int_size());
  EXPECT_EQ(1, test_message.rep_int(0));
  EXPECT_EQ(2, test_message.rep_int(1));

  ASSERT_EQ(2, test_message.test_enums_size());
  EXPECT_EQ(TestMessage::V1, test_message.test_enums(0));
  EXPECT_EQ(TestMessage::V2, test_message.test_enums(1));

  ASSERT_EQ(2, test_message.bs_size());
  EXPECT_EQ(m_binary_data_bs1, test_message.bs(0));
  EXPECT_EQ(m_binary_data_bs2, test_message.bs(1));

  const Reflection* reflection = test_message.GetReflection();
  const FieldDescriptor* field_query_ext =
    reflection->FindKnownExtensionByName("pb_json_test.query_ext");
  ASSERT_TRUE(field_query_ext);
  EXPECT_EQ("含义query",
            reflection->GetString(test_message, field_query_ext));
  const FieldDescriptor* field_long_ext =
    reflection->FindKnownExtensionByName("pb_json_test.long_ext");
  ASSERT_TRUE(field_long_ext);
  EXPECT_EQ(0xFFFFFFFFFFFFFFFFull,
            reflection->GetUInt64(test_message, field_long_ext));
  EXPECT_FALSE(test_message.has_test_num());

  StringPiece json_data_str_enum(kJsonString3);
  TestMessage test_message_str_enum;
  ASSERT_TRUE(JsonToProtoMessage(json_data_str_enum, &test_message_str_enum));
  EXPECT_EQ(TestMessage::V2, test_message_str_enum.test_enum());
  ASSERT_EQ(2, test_message_str_enum.test_enums_size());
  EXPECT_EQ(TestMessage::V1, test_message_str_enum.test_enums(0));
  EXPECT_EQ(TestMessage::V2, test_message_str_enum.test_enums(1));

  StringPiece json_data_str_enum2(kJsonString4);
  TestMessage test_message_str_enum2;
  ASSERT_TRUE(JsonToProtoMessage(json_data_str_enum2, &test_message_str_enum2));
  EXPECT_EQ(TestMessage::V2, test_message_str_enum2.test_enum());
  ASSERT_EQ(2, test_message_str_enum2.test_enums_size());
  EXPECT_EQ(TestMessage::V1, test_message_str_enum2.test_enums(0));
  EXPECT_EQ(TestMessage::V2, test_message_str_enum2.test_enums(1));
}

TEST_F(PbJsonTest, InvalidJsonToProtoMessage) {
  string invalid_json_str = "\"" + kJsonString + "\"";
  StringPiece json_data(invalid_json_str);
  TestMessage test_message;
  ASSERT_FALSE(JsonToProtoMessage(json_data, &test_message));
}

}  // namespace gdt
