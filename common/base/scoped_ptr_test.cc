// Copyright    (C), 1998-2012, Tencent
// Author: jefftang@tencent.com
// Date: 2012-11-22

#include "common/base/scoped_ptr.h"

#include <algorithm>
#include "common/base/static_assert.h"
#include "thirdparty/gtest/gtest.h"
#include "thirdparty/glog/logging.h"

namespace gdt {

// All this class does is keep track of how many instances of it are
// constructed & destructed.
class LifeEventCounted {
 public:
  LifeEventCounted() { ++construction_count_; }
  ~LifeEventCounted() { ++deletion_count_; }

  static void ClearCounts() { construction_count_ = deletion_count_ = 0; }

  static int construction_count() { return construction_count_; }
  static int deletion_count() { return deletion_count_; }
  LifeEventCounted* addr() { return this; }

 private:
  static int construction_count_;
  static int deletion_count_;
};

int LifeEventCounted::construction_count_ = 0;
int LifeEventCounted::deletion_count_ = 0;

void Test_make_scoped_ptr(LifeEventCounted *lec);

class ScopedPtrTest : public testing::Test {
};

TEST_F(ScopedPtrTest, scoped_ptr) {
  LifeEventCounted::ClearCounts();
  LifeEventCounted* lec1 = new LifeEventCounted;
  LifeEventCounted* lec2 = new LifeEventCounted;
  LifeEventCounted* lec3 = new LifeEventCounted;
  LifeEventCounted* lec4 = new LifeEventCounted;
  // Easiest way to shut up compiler
  std::cout << "lec4: " << lec4 << std::endl;
  {
    scoped_ptr<LifeEventCounted> ptr1(lec1);
    ASSERT_TRUE(ptr1 != NULL);
    scoped_ptr<LifeEventCounted> ptr2(lec2);
    scoped_ptr<LifeEventCounted> empty_ptr;
    ASSERT_TRUE(empty_ptr == NULL);
    scoped_ptr<LifeEventCounted> assigned_ptr1;
    assigned_ptr1.reset(lec3);
    LifeEventCounted& ref = *ptr1;
    ASSERT_TRUE(&ref == lec1);
    ASSERT_TRUE(assigned_ptr1.get() == lec3);
    ASSERT_TRUE(assigned_ptr1->addr() == lec3);

    if (lec4 != ptr2) {
      LifeEventCounted* lec5 = new LifeEventCounted;
      scoped_ptr<const LifeEventCounted> ptr5(lec5);
      if (lec3 != ptr5)  {
        LOG(INFO) << "lec3 != ptr5";
      }
    }
    swap(ptr1, ptr2);
    ASSERT_TRUE(ptr2.get() == lec1);
    ASSERT_TRUE(ptr1.get() == lec2);
    ASSERT_TRUE(ptr2 == lec1);
    ASSERT_TRUE(lec2 == ptr1);
    ASSERT_TRUE(ptr1 != lec1);
    ASSERT_TRUE(lec2 != ptr2);
  }
  ASSERT_EQ(LifeEventCounted::construction_count(), 5);
  ASSERT_EQ(LifeEventCounted::deletion_count(), 4);

  Test_make_scoped_ptr(make_scoped_ptr(lec4).get());

  ASSERT_EQ(LifeEventCounted::construction_count(), 5);
  ASSERT_EQ(LifeEventCounted::deletion_count(), 5);
  scoped_ptr<int> test_size_var(new int);
  int *p_int = NULL;
  STATIC_ASSERT(sizeof(test_size_var) == sizeof(p_int),
                "scoped ptr should have no overhead");
}

void Test_make_scoped_ptr(LifeEventCounted *lec) {
  ASSERT_EQ(LifeEventCounted::construction_count(), 5);
  ASSERT_EQ(LifeEventCounted::deletion_count(), 4);
}

TEST_F(ScopedPtrTest, scoped_array) {
  LifeEventCounted::ClearCounts();

  LifeEventCounted* lec1 = new LifeEventCounted[2];
  LifeEventCounted* lec2 = new LifeEventCounted[3];
  LifeEventCounted* lec3 = new LifeEventCounted[6];
  LifeEventCounted* lec4 = new LifeEventCounted[12];
  // Easiest way to shut up compiler
  std::cout << "lec4: " << lec4 << std::endl;
  {
    scoped_array<LifeEventCounted> ptr1(lec1);
    ASSERT_TRUE(ptr1 != NULL);
    scoped_array<LifeEventCounted> ptr2(lec2);
    scoped_array<LifeEventCounted> empty_ptr;
    ASSERT_TRUE(empty_ptr == NULL);
    scoped_array<LifeEventCounted> assigned_ptr1;
    assigned_ptr1.reset(lec3);
    LifeEventCounted& ref = ptr2[2];
    ASSERT_TRUE(&ref == lec2+2);
    ASSERT_TRUE(assigned_ptr1.get() == lec3);

    swap(ptr1, ptr2);
    ASSERT_TRUE(ptr2.get() == lec1);
    ASSERT_TRUE(ptr1.get() == lec2);
    ASSERT_TRUE(ptr2 == lec1);
    ASSERT_TRUE(lec2 == ptr1);
    ASSERT_TRUE(ptr1 != lec1);
    ASSERT_TRUE(lec2 != ptr2);
  }
  ASSERT_EQ(LifeEventCounted::construction_count(), 23);
  ASSERT_EQ(LifeEventCounted::deletion_count(), 11);
  delete [] lec4;
  scoped_ptr<int> test_size_var(new int);
  int *p_int = NULL;
  STATIC_ASSERT(sizeof(test_size_var) == sizeof(p_int),
                "scoped array should have no overhead");
}

static bool file_closed = false;

static int CloseFile(FILE* fp) {
  file_closed = true;
  return fclose(fp);
}

namespace {
struct FileCloser {
  void operator()(FILE* fp) {
    CloseFile(fp);
  }
};
}  // namespace

TEST_F(ScopedPtrTest, WithDeleter) {
  {
    file_closed = false;
    scoped_ptr<FILE, FileCloser> fp1(fopen("/dev/null", "rb"));
    ASSERT_EQ(sizeof(FILE*), sizeof(fp1));
  }
  EXPECT_TRUE(file_closed);

  {
    file_closed = false;
    scoped_ptr<FILE, int (*)(FILE*)> fp2(fopen("/dev/null", "rb"), CloseFile);
    ASSERT_GT(sizeof(fp2), sizeof(FILE*));
  }
  EXPECT_TRUE(file_closed);
}

TEST_F(ScopedPtrTest, BoolTest) {
  scoped_ptr<int> p1(new int(0));
  scoped_ptr<int> p2;
  EXPECT_TRUE(p1);
  EXPECT_FALSE(p2);
  EXPECT_FALSE(!p1);
  EXPECT_TRUE(!p2);
}

TEST_F(ScopedPtrTest, ArrayBoolTest) {
  scoped_array<int> p1(new int[1]);
  scoped_array<int> p2;
  EXPECT_TRUE(p1);
  EXPECT_FALSE(p2);
  EXPECT_FALSE(!p1);
  EXPECT_TRUE(!p2);
}

}  // namespace gdt
