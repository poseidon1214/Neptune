// Copyright (C), 1998-2012, Tencent
// Author: jefftang@tencent.com
// Date: 2013-3-5

#include <algorithm>
#include <utility>
#include "app/qzap/common/utility/intrusive_list.h"
#include "thirdparty/gtest/gtest.h"
namespace {
class IntrusiveListTest : public testing::Test {
 protected:
  virtual void SetUp() {
  }

  virtual void TearDown() {
  }

  static const int kLoopNumber = 1000;
};

struct TestItem {
  intrusive_link<TestItem> link0;
  int n;
  intrusive_link<TestItem> link;
  intrusive_link<TestItem> link1;
  intrusive_link<TestItem> link2;
};

typedef intrusive_list<TestItem> TestList;
typedef intrusive_list<TestItem, &TestItem::link0> TestList0;
typedef intrusive_list<TestItem, &TestItem::link1> TestList1;
typedef intrusive_list<TestItem, &TestItem::link2> TestList2;

TEST_F(IntrusiveListTest, Basic) {
  TestList list1;

  for (int i = 0; i < 10; ++i) {
    TestItem *e = new TestItem;
    e->n = i;
    list1.push_front(e);
  }
  ASSERT_EQ(list1.size(), 10u);

  // Verify we can reverse a list because we defined swap for TestItem.
  std::reverse(list1.begin(), list1.end());
  ASSERT_EQ(list1.size(), 10u);

  // Check both const and non-const forward iteration.
  int i = 0;
  TestList::iterator iter = list1.begin();
  for (;
       iter != list1.end();
       ++iter, ++i) {
    ASSERT_TRUE(iter.valid());
    ASSERT_EQ(iter->n, i);
  }
  ASSERT_TRUE(iter == list1.end());
  ASSERT_TRUE(iter != list1.begin());
  i = 0;
  iter = list1.begin();
  for (;
       iter != list1.end();
       ++iter, ++i) {
    ASSERT_TRUE(iter.valid());
    ASSERT_EQ(iter->n, i);
  }
  ASSERT_TRUE(iter == list1.end());
  ASSERT_TRUE(iter != list1.begin());

  ASSERT_EQ(list1.front().n, 0);
  ASSERT_EQ(list1.back().n, 9);

  // Verify we can swap 2 lists.
  TestList list2;
  list2.swap(list1);
  ASSERT_EQ(list1.size(), 0u);
  ASSERT_EQ(list2.size(), 10u);

  // Check both const and non-const reverse iteration.
  const TestList& clist2 = list2;
  TestList::reverse_iterator riter = list2.rbegin();
  i = 9;
  for (;
       riter != list2.rend();
       ++riter, --i) {
    ASSERT_TRUE(iter.valid());
    ASSERT_EQ(riter->n, i);
  }
  ASSERT_TRUE(riter == clist2.rend());
  ASSERT_TRUE(riter != clist2.rbegin());

  riter = list2.rbegin();
  i = 9;
  for (;
       riter != list2.rend();
       ++riter, --i) {
    ASSERT_TRUE(iter.valid());
    ASSERT_EQ(riter->n, i);
  }
  ASSERT_TRUE(riter == clist2.rend());
  ASSERT_TRUE(riter != clist2.rbegin());

  while (!list2.empty()) {
    TestItem *e = &list2.front();
    list2.pop_front();
    delete e;
  }
}

TEST_F(IntrusiveListTest, Erase) {
  TestList l;
  TestItem *e[10];

  // Create a list with 10 items.
  for (int i = 0; i < 10; ++i) {
    e[i] = new TestItem;
    l.push_front(e[i]);
  }

  // Test that erase works.
  for (int i = 0; i < 10; ++i) {
    ASSERT_EQ(l.size(), static_cast<size_t>(10 - i));
    l.erase(e[i]);
    ASSERT_EQ(l.size(), static_cast<size_t>(10 - i - 1));
    delete e[i];
  }
}

TEST_F(IntrusiveListTest, Insert) {
  TestList l;
  TestList::iterator iter = l.end();
  TestItem *e[10];

  // Create a list with 10 items.
  for (int i = 9; i >= 0; --i) {
    e[i] = new TestItem;
    iter = l.insert(iter, e[i]);
    ASSERT_EQ(&(*iter), e[i]);
  }

  ASSERT_EQ(l.size(), 10u);

  // Verify insertion order.
  iter = l.begin();
  for (int i = 0; i < 10; ++i) {
    ASSERT_EQ(&(*iter), e[i]);
    iter = l.erase(e[i]);
    delete e[i];
  }
}

TEST_F(IntrusiveListTest, StaticInsertErase) {
  TestList l;
  TestItem e[2];
  TestList::iterator i = l.begin();
  l.insert(i, &e[0]);
  l.insert(&e[0], &e[1]);
  l.erase(&e[0]);
  l.erase(&e[1]);
  EXPECT_TRUE(l.empty());
}

TEST_F(IntrusiveListTest, NullToInt) {
  // The intrusive_list<>::end_node() implementation depends on a null pointer
  // casting to the integer value 0. The standard doesn't guarantee this and
  // apparently it isn't true on some implementations.
  intptr_t val = reinterpret_cast<intptr_t>(static_cast<void*>(0));
  ASSERT_EQ(val, 0);
}

TEST_F(IntrusiveListTest, TestCopyIntrusiveList) {
  struct TestData {
    TestItem items[kLoopNumber];
    TestList0 list0;
    TestList1 list1;
    TestList2 list2;
  };
  TestData d1;
  TestList0 *list0 = &d1.list0;
  TestList1 *list1 = &d1.list1;
  TestList2 *list2 = &d1.list2;
  for (int i = 0; i < kLoopNumber; ++i) {
    d1.items[i].n = i;
    list0->push_back(&d1.items[i]);
    list1->push_back(&d1.items[i]);
    list2->push_front(&d1.items[i]);
  }

  int i = 0;
  for (TestList1::iterator iter = list1->begin();
       iter != list1->end(); ++iter) {
    ASSERT_TRUE(iter.valid());
    ASSERT_EQ(iter->n, i++);
  }

  for (TestList2::iterator iter = list2->begin();
       iter != list2->end(); ++iter) {
    ASSERT_TRUE(iter.valid());
    ASSERT_EQ(iter->n, --i);
  }
  for (TestList2::iterator iter = list2->begin();
       iter != list2->end();) {
    ASSERT_TRUE(iter.valid());
    TestList2::iterator next_iter = iter;
    ++next_iter;
    switch (iter->n % 3) {
      case 0:
        d1.list1.erase(&*iter);
        d1.list2.erase(&*iter);
        break;
      case 1:
        d1.list0.erase(&*iter);
        d1.list2.erase(&*iter);
        break;
      default:
        d1.list0.erase(&*iter);
        d1.list1.erase(&*iter);
    }
    iter = next_iter;
  }

  for (TestList0::iterator iter = list0->begin();
       iter != list0->end(); ++iter) {
    ASSERT_TRUE(iter.valid());
    EXPECT_EQ(iter->n % 3, 0);
  }

  for (TestList1::iterator iter = list1->begin();
       iter != list1->end(); ++iter) {
    ASSERT_TRUE(iter.valid());
    EXPECT_EQ(iter->n % 3, 1);
  }

  for (TestList2::iterator iter = list2->begin();
       iter != list2->end(); ++iter) {
    ASSERT_TRUE(iter.valid());
    EXPECT_EQ(iter->n % 3, 2);
  }

  // Test the copy.
  TestData d2 = d1;
  list0 = &d2.list0;
  list1 = &d2.list1;
  list2 = &d2.list2;

  for (TestList0::iterator iter = list0->begin();
       iter != list0->end(); ++iter) {
    ASSERT_TRUE(iter.valid());
    ASSERT_EQ(iter->n % 3, 0);
  }

  for (TestList1::iterator iter = list1->begin();
       iter != list1->end(); ++iter) {
    ASSERT_TRUE(iter.valid());
    ASSERT_EQ(iter->n % 3, 1);
  }

  for (TestList2::iterator iter = list2->begin();
       iter != list2->end(); ++iter) {
    ASSERT_TRUE(iter.valid());
    ASSERT_EQ(iter->n % 3, 2);
  }
}

TEST_F(IntrusiveListTest, TestCopyIntrusiveListIterator) {
  struct TestData {
    TestItem items[kLoopNumber];
    TestList0 list0;
    TestList1 list1;
    TestList2 list2;
    int hand;
  };
  TestData d1;
  TestList0 *list0 = &d1.list0;
  TestList1 *list1 = &d1.list1;
  TestList2 *list2 = &d1.list2;
  for (int i = 0; i < kLoopNumber; ++i) {
    d1.items[i].n = i;
    list0->push_back(&d1.items[i]);
    list1->push_back(&d1.items[i]);
    list2->push_front(&d1.items[i]);
  }

  int i = 0;
  for (TestList1::iterator iter = list1->begin();
       iter != list1->end(); ++iter) {
    ASSERT_TRUE(iter.valid());
    ASSERT_EQ(iter->n, i++);
  }

  for (TestList2::iterator iter = list2->begin();
       iter != list2->end(); ++iter) {
    ASSERT_EQ(iter->n, --i);
  }
  for (TestList2::iterator iter = list2->begin();
       iter != list2->end();) {
    ASSERT_TRUE(iter.valid());
    TestList2::iterator next_iter = iter;
    ++next_iter;
    switch (iter->n % 3) {
      case 0:
        d1.list1.erase(&*iter);
        d1.list2.erase(&*iter);
        break;
      case 1:
        d1.list0.erase(&*iter);
        d1.list2.erase(&*iter);
        break;
      default:
        d1.list0.erase(&*iter);
        d1.list1.erase(&*iter);
    }
    iter = next_iter;
  }

  for (TestList0::iterator iter = list0->begin();
       iter != list0->end(); ++iter) {
    ASSERT_TRUE(iter.valid());
    EXPECT_EQ(iter->n % 3, 0);
  }

  for (TestList1::iterator iter = list1->begin();
       iter != list1->end(); ++iter) {
    ASSERT_TRUE(iter.valid());
    EXPECT_EQ(iter->n % 3, 1);
  }

  for (TestList2::iterator iter = list2->begin();
       iter != list2->end(); ++iter) {
    ASSERT_TRUE(iter.valid());
    EXPECT_EQ(iter->n % 3, 2);
  }

  int count_for_i0 = 0;
  int total_sum = 0;
  TestList0::iterator i0;
  for (i0 = list0->begin(); i0 != list0->end(); ++i0) {
    if (i0->n == 27) {
      d1.hand = &*i0 - &d1.items[0];
      break;
    }
    count_for_i0 += i0->n;
    total_sum += i0->n;
  }
  ASSERT_EQ(i0->n, 27);
  int count_for_ii0 = 0;
  for (i0 = list0->begin(); i0 != list0->iterator_from(&d1.items[d1.hand]);
       ++i0) {
    count_for_ii0 += i0->n;
  }
  ASSERT_EQ(count_for_i0, count_for_ii0);
  for (i0 = list0->iterator_from(&d1.items[d1.hand]); i0 != list0->end();
       ++i0) {
    total_sum += i0->n;
  }

  // Test the copy.
  TestData d2 = d1;
  list0 = &d2.list0;
  list1 = &d2.list1;
  list2 = &d2.list2;

  for (TestList0::iterator iter = list0->begin();
       iter != list0->end(); ++iter) {
    ASSERT_TRUE(iter.valid());
    ASSERT_EQ(iter->n % 3, 0);
  }

  for (TestList1::iterator iter = list1->begin();
       iter != list1->end(); ++iter) {
    ASSERT_TRUE(iter.valid());
    ASSERT_EQ(iter->n % 3, 1);
  }

  for (TestList2::iterator iter = list2->begin();
       iter != list2->end(); ++iter) {
    ASSERT_TRUE(iter.valid());
    ASSERT_EQ(iter->n % 3, 2);
  }

  TestList0::iterator i20 = list0->iterator_from(&d2.items[d2.hand]);
  TestList0::reverse_iterator reverse_i20 = list0->reverse_iterator_from(
      &d2.items[d2.hand]);
  ASSERT_EQ(i20->n, 27);
  count_for_ii0 = 0;
  for (i0 = list0->begin(); i0 != i20; ++i0) {
    count_for_ii0 += i0->n;
  }

  int count_for_ri = 0;
  for (TestList0::reverse_iterator ri = reverse_i20; ri != list0->rend();
       ++ri) {
    count_for_ri += ri->n;
  }

  ASSERT_EQ(count_for_ri, count_for_ii0);
  ASSERT_EQ(count_for_ii0, count_for_i0);
  for (i0 = i20; i0 != list0->end(); ++i0) {
    count_for_ii0 += i0->n;
  }
  for (TestList0::reverse_iterator ri = list0->rbegin(); ri != reverse_i20;
       ++ri) {
    count_for_ri += ri->n;
  }

  ASSERT_EQ(total_sum, count_for_ri);
  ASSERT_EQ(total_sum, count_for_ii0);

}

TEST_F(IntrusiveListTest, TestContextCrash) {
  struct TestData {
    TestItem items[kLoopNumber];
    TestList0 list0;
    TestList1 list1;
    TestList2 list2;
  };
  TestData d1;
  TestList0 *list0 = &d1.list0;
  for (int i = 0; i < kLoopNumber; ++i) {
    d1.items[i].n = i;
    list0->push_back(&d1.items[i]);
  }

  for (TestList0::iterator iter = list0->begin();
       iter != list0->end(); ++iter) {
    ASSERT_TRUE(iter.valid());
    ASSERT_FALSE(iter.node() == NULL);
  }

  memset(&d1.items[10].link0, 0xdeadbeef, sizeof(d1.items[0].link0));
  int i = 0;
  for (TestList0::iterator iter = list0->begin();
       iter != list0->end(); ++iter) {
    if (i++ <= 10) {
      ASSERT_TRUE(iter.valid());
      ASSERT_FALSE(iter.node() == NULL);
    } else {
      ASSERT_FALSE(iter.valid());
      ASSERT_TRUE(iter.node() == NULL);
      break;
    }
  }
  // Test the copy.
  TestData d2 = d1;
  list0 = &d2.list0;

  i = 0;
  for (TestList0::iterator iter = list0->begin();
       iter != list0->end(); ++iter) {
    if (i++ <= 10) {
      ASSERT_TRUE(iter.valid());
      ASSERT_FALSE(iter.node() == NULL);
    } else {
      ASSERT_FALSE(iter.valid());
      ASSERT_TRUE(iter.node() == NULL);
      break;
    }
  }
}
}  // namespace

namespace std {
template <>
void swap(TestItem &a, TestItem &b) {
  swap(a.n, b.n);
}
}
