// Copyright (c) 2012 Tencent Inc.
// Author: Pan Yang (baileyyang@tencent.com)

#include "app/qzap/text_analysis/classifier/classifier.h"

#include "thirdparty/gflags/gflags.h"
#include "thirdparty/glog/logging.h"
#include "thirdparty/gtest/gtest.h"
#include "app/qzap/text_analysis/text_miner.pb.h"

namespace qzap {
namespace text_analysis {

class ClassifierTest : public ::testing::Test {
 public:
  virtual void SetUp() {
    classifier_.reset(new Classifier());
    ASSERT_TRUE(classifier_->LoadModel("testdata/classifier_model"));
  }

 protected:
  scoped_ptr<Classifier> classifier_;
};

TEST_F(ClassifierTest, EmptyPredict) {
  Document document;
  Field* field = document.add_field();
  field->set_text("");
  EXPECT_TRUE(classifier_->Predict(&document));
  EXPECT_EQ(0, document.category_size());
}

}  // namespace text_analysis
}  // namespace qzap

int main(int argc, char** argv) {
  google::InitGoogleLogging(argv[0]);
  testing::InitGoogleTest(&argc, argv);
  google::ParseCommandLineFlags(&argc, &argv, true);

  return RUN_ALL_TESTS();
}

