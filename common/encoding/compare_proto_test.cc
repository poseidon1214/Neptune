// Copyright (c) 2013, Tencent Inc.
// All rights reserved.
//
// Author: Huan Yu <huanyu@tencent.com>

#include "common/encoding/compare_proto.h"

#include <set>
#include "common/encoding/find_field_test.pb.h"
#include "thirdparty/gtest/gtest.h"

namespace gdt {

using gdt::test::Person;

class ProtoCompareTest : public ::testing::Test {
 protected:
  virtual void SetUp() {
    person_alpha_.set_name("Yi Wang");
    person_alpha_.set_age(0);
    person_alpha_.add_gender(Person::MALE);
    person_alpha_.add_address("Yinke Building");
    person_alpha_.add_address("Huilongguan");

    person_beta_.set_name("Yi Wang");
    person_beta_.add_gender(Person::MALE);
    person_beta_.add_address("Yinke Building");
    person_beta_.add_address("Huilongguan");
  }

  Person person_alpha_;
  Person person_beta_;
};

TEST_F(ProtoCompareTest, EquivalentButNotEqual) {
  EXPECT_TRUE(ProtoEquivalent(person_alpha_, person_beta_));
  EXPECT_FALSE(ProtoEqual(person_alpha_, person_beta_));
}

TEST_F(ProtoCompareTest, NestedMessages) {
  Person alpha;
  Person beta;
  alpha.CopyFrom(person_alpha_);
  beta.CopyFrom(person_beta_);
  alpha.mutable_couple()->CopyFrom(person_beta_);
  beta.mutable_couple()->CopyFrom(person_alpha_);
  EXPECT_TRUE(ProtoEquivalent(alpha, beta));
}

TEST_F(ProtoCompareTest, ProtoCompare) {
  Person alpha;
  Person beta;
  alpha.set_name("yuhuan");
  beta.set_name("yuhuan");
  EXPECT_EQ(0, ProtoCompare(alpha, beta));
  alpha.set_name("ABC");
  beta.set_name("DEF");
  EXPECT_LT(ProtoCompare(alpha, beta), 0);
  EXPECT_GT(ProtoCompare(beta, alpha), 0);
}

TEST_F(ProtoCompareTest, ProtoLess) {
  std::set<Person, ProtoLess> s;
  s.insert(person_alpha_);
  s.insert(person_alpha_);
  ASSERT_EQ(1U, s.size());
  s.insert(person_beta_);
  ASSERT_EQ(2U, s.size());
}

TEST_F(ProtoCompareTest, ProtoGreater) {
  std::set<Person, ProtoGreater> s;
  s.insert(person_alpha_);
  s.insert(person_alpha_);
  ASSERT_EQ(1U, s.size());
  s.insert(person_beta_);
  ASSERT_EQ(2U, s.size());
}

}  // namespace gdt
