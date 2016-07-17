// Copyright (C), 1998-2012, Tencent
// Author: jefftang@tencent.com
// Date: 2013-3-5
// The intrusive_list at here is a slightly different to its original version,
// from such a usage:
//   struct TestData {
//     int n;
//     instrusive_link<TestData> link;
//   };
//   struct TestItem {
//     TestData data[100];
//     instrusive_list<TestItem> list;
//   };
//   TestItem i1, i2;
//   Manipulates i1;
//   Writes memory block of i1 to disk.
//   Loads memory block of i1 from disk to i2
//   then the i2 have the same list layout as i1, this feature is important for
//   sharing the information through share memory between processes.
//   You can't do this in the original intrusive_list, because it is base on
//   memory pointer.
// An intrusive_list<> is a doubly-linked list where the link pointers are
// embedded in the items stored in the list. Intrusive lists are circularly
// linked making inserting and removing an element constant time (and
// branch-free) operations if you already have the position you want to
// insert/remove at. Usage is similar to an STL list<>, with the addition of
// needing to embed an intrusive_link<> structure in the items you want to
// store in your intrusive_list<>.
//
//   struct Foo {
//     intrusive_link<Foo> link;
//   };
//   typedef intrusive_list<Foo, &Foo::link> FooList;
//
//   FooList l;
//   l.push_back(new Foo);
//   l.push_front(new Foo);
//   l.erase(&l.front());
//   l.erase(&l.back());
//
// Intrusive lists are primarily useful when you would have considered
// embedding link pointers in your class directly for space or performance
// reasons. An intrusive_link<> is the size of 2 pointers, usually 8
// bytes. Intrusive lists do not perform memory allocation (unlike the STL
// list<>) class and thus may use less memory than list<>. In particular, if
// the list elements are pointers to objects, using a list<> would perform an
// extra memory allocation for each list node structure, while an
// intrusive_list<> would not.
//
// Because the list pointers are embedded in the objects stored in an
// intrusive_list<>, erasing an item from a list is constant time. Consider the
// following:
//
//   map<string,Foo> foo_map;
//   list<Foo*> foo_list;
//
//   foo_list.push_back(&foo_map["bar"]);
//   foo_list.erase(&foo_map["bar"]); // Compile error!
//
// The problem here is that a Foo* doesn't know where on foo_list it resides,
// so removal requires iteration over the list. Various tricks can be performed
// to overcome this. For example, a foo_list::iterator can be stored inside of
// the Foo object. But at that point you'd be better of using an
// intrusive_list<>:
//
//   map<string,Foo> foo_map;
//   instrusive_list<Foo> foo_list;
//
//   foo_list.push_back(&foo_map["bar"]);
//   foo_list.erase(&foo_map["bar"]); // Yeah!
//
// Note that intrusive_lists come with a few limitations. The primary
// limitation is that the intrusive_link<> structure is not copyable or
// assignable. The result is that STL algorithms which mutate the order of
// iterators, such as reverse() and unique(), will not work by default with
// intrusive_lists. In order to allow these algorithms to work you'll need to
// define swap() and/or operator= for your class. And these routines must not
// swap or assign to the link pointers.
//
// Another limitation is that the intrusive_list<> structure itself is not
// copyable or assignable since an item/link combination can only exist on one
// intrusive_list<> at a time. This limitation is a result of the link pointers
// for an item being intrusive in the item itself. For example, the following
// will not compile:
//
//   FooList a;
//   FooList b(a); // no copy constructor
//   b = a;        // no assignment operator
//
// The similar STL code does work since the link pointers are external to the
// item:
//
//   list<int*> a;
//   a.push_back(new int);
//   list<int*> b(a);
//   CHECK(a.front() == b.front());
#ifndef APP_QZAP_COMMON_UTILITY_INTRUSIVE_LIST_H_
#define APP_QZAP_COMMON_UTILITY_INTRUSIVE_LIST_H_

#include <stddef.h>
#include <stdint.h>
#include <iterator>

// The intrusive_link structure provides the next and previous pointers for an
// intrusive list item. The structure is intended to be intrusive in the class
// you want to store on an intrusive_list<>. See the example above.
// The base parameters in the functions, is the address of the intrusive_list
// object.
// The two variables, next_offset_ and prev_offset_, is the offset of entry to
// the intrusive_list, so, if the layout between entries and instrusive_list is
// not changed, we can safely copy that memory block as well maintaining the
// list layout.
template <class T>
struct intrusive_link {
 private:
  const T *OffsetToPtr(const void *base, ptrdiff_t offset) const {
    return reinterpret_cast<const T*>(
        reinterpret_cast<const char *>(base) + offset);
  }

  T *OffsetToPtr(void* base, ptrdiff_t offset) {
    return reinterpret_cast<T*>(
        reinterpret_cast<char *>(base) + offset);
  }

  ptrdiff_t PtrToOffset(const void *base, const T *t) const {
    return reinterpret_cast<const char *>(t) -
      reinterpret_cast<const char *>(base);
  }
 public:
  void set_next(const void *base, const T *next) {
    if (!valid()) {
      return;
    }
    next_offset_ = PtrToOffset(base, next);
    UpdateMagic();
  }

  void set_prev(const void *base, const T *prev) {
    if (!valid()) {
      return;
    }
    prev_offset_ = PtrToOffset(base, prev);
    UpdateMagic();
  }

  const T *next(const void *base) const {
    return valid() ? OffsetToPtr(base, next_offset_) : NULL;
  }

  T *next(void *base) {
    return valid() ? OffsetToPtr(base, next_offset_) : NULL;
  }

  T *prev(void *base) {
    return valid() ? OffsetToPtr(base, prev_offset_) : NULL;
  }

  intrusive_link() {
    reset();
  }

  void reset() {
    next_offset_ = prev_offset_ = 0;
    UpdateMagic();
  }
 private:
  bool valid() const {
    return (backup_next_offset_ ^ kMagic) == next_offset_ &&
      (backup_prev_offset_ ^ kMagic) == prev_offset_;
  }

  void UpdateMagic() {
    backup_next_offset_ = next_offset_ ^ kMagic;
    backup_prev_offset_ = prev_offset_ ^ kMagic;
  }
  static const uint64_t kMagic = 0xdeadbeef;
  uint64_t next_offset_;
  uint64_t prev_offset_;
  uint64_t backup_next_offset_;
  uint64_t backup_prev_offset_;
};

template <class T, intrusive_link<T> (T::*link_ptr) = &T::link>
class intrusive_list {
 private:
  typedef intrusive_list<T, link_ptr> intrusive_list_type;
  // The structure used for iterating over an intrusive_list.
  template <class BasePtr, class Ref, class Ptr>
  struct intrusive_list_iterator {
    typedef intrusive_list_iterator<BasePtr, T&, T*> iterator;
    typedef intrusive_list_iterator<const BasePtr, const T&, const T*>
      const_iterator;
    typedef intrusive_list_iterator<BasePtr, Ref, Ptr> self;

    typedef std::bidirectional_iterator_tag iterator_category;
    typedef T value_type;
    typedef Ptr pointer;
    typedef const T *const_pointer;
    typedef Ref reference;
    typedef const T &const_reference;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
    typedef BasePtr base_pointer;

    intrusive_list_iterator() : node_(NULL) { }
    intrusive_list_iterator(pointer x, base_pointer y) : node_(x), base_(y) { }
    intrusive_list_iterator(const iterator &x)
        : node_(x.node_), base_(x.base_) { }

    template <class BasePtr1, class Ref1, class Ptr1>
    intrusive_list_iterator(
      const intrusive_list_iterator<BasePtr1, Ref1, Ptr1> &iterator)
      : node_(iterator.node_), base_(iterator.base_) {
    }

    template <class BasePtr1, class Ref1, class Ptr1>
    bool operator == (
        const intrusive_list_iterator<BasePtr1, Ref1, Ptr1>  &x) const {
      return node_ == x.node_ && base_ == x.base_;
    }

    template <class BasePtr1, class Ref1, class Ptr1>
    bool operator != (
        const intrusive_list_iterator<BasePtr1, Ref1, Ptr1>  &x) const {
      return !(*this == x);
    }

    reference operator*() const { return *node_; }
    pointer operator->() const { return node_; }

    self& operator++() {
      node_ = (node_->*link_ptr).next(base_);
      return *this;
    }

    self  operator++(int) {
      self tmp = *this;
      ++*this;
      return tmp;
    }

    self& operator--() {
      node_ = (node_->*link_ptr).prev(base_);
      return *this;
    }

    self  operator--(int) {
      self tmp = *this; --*this;
      return tmp;
    }

    bool valid() const {
      return node_ != NULL;
    }

    pointer node() const {
      return node_;
    }
   private:
    friend class intrusive_list;
    // The node_ pointer needs to be public so that we can access it from
    // different intrusive_list_iterator<> specializations. (e.g. From the
    // copy-constructor from an iterator to a const_iterator).
    pointer node_;
    // The address of intrusive list.
    base_pointer base_;
  };

  // Okay, here is the hokey and not quite safe part of the intrusive_list<>
  // implementation. Basically, we want to construct a T* so that when we
  // dereference the pointer-to-data-member (i.e. link_ptr) we'll arrive back
  // at "this". Huh? We just want the following to work:
  //
  //   intrusive_list<T,link_ptr> list;
  //   T *obj = list.end_node();
  //   assert(&(obj->*link_ptr) == &list);
  //
  // The reason for wanting this is that intrusive_list<T> is a circularly
  // linked list with "this" acting as the sentinel node at the end of the
  // list. But "this" is of type intrusive_list<T> and not T. Which is where
  // the hokey part comes in. The way pointers-to-data-members are implemented
  // for "complex" classes isn't necessarily a simple integer offset. Applying
  // the pointer-to-data-member (i.e. doing "obj->*link_ptr") for a virtual
  // base data member to a derived pointer will need to look inside "obj" to
  // find the offset of the virtual base. Consider the following:
  //
  // struct virtual_base {
  //   int x;
  // };
  //
  // struct derived : public virtual virtual_base {
  //   int offset_of(int virtual_base::*member_ptr) {
  //     return reinterpret_cast<char*>(&this->*member_ptr) -
  //         reinterpret_cast<char*>(this);
  //   }
  // };
  //
  // When "member_ptr" is applied to "this" in derived::offset_of(), the
  // compiler first casts "this" to a "virtual_base". Unlike for non-virtual
  // base classes, this involves looking inside of "this" to determine the
  // offset of the virtual base class from "this". (For non-virtual base
  // classes, the offset of a parent class can be determined at compile
  // time). In the above example, the cast from "this" to "virtual_base*" works
  // fine. The trickery below produces a fake T* which is not a pointer to a
  // valid T object. Looking inside this fake T* would be bad and could cause
  // crashes or other misbehavior. On the bright side, the trickery below
  // detects at runtime when application of the "link_ptr" needs to look inside
  // the object by applying "link_ptr" to "reinterpret_cast<T*>(0)". If
  // application of "link_ptr" needs to look inside the "T*", we'll get a nice
  // NULL pointer dereference. Even better, because "link_ptr" is a "T::*"
  // pointer-to-data-member, application of "link_ptr" to a "T*" does not need
  // to implicitly cast "T*" to a different type so the construction of
  // "end_node()" should always be safe.

  // Returns a pointer to the "end" node of the intrusive_list. This is a
  // specially constructed T* which is really a pointer to "this". See the
  // discussion above.
  T* end_node() const {
    // This effectively calculates offsetof(T, link_ptr) by first creating a
    // fake T* from address 0 and applying link_ptr to find the address of the
    // link structure, and then casting the result to an integer type. Note
    // that a reasonable compiler (e.g. gcc-4) can compute this at compile time
    // and will end up performing only the link_offset subtraction in this
    // method at runtime.

    // Note that the following statement makes two assumptions that might be
    // problematic someday: a) assumes that a null pointer, cast to int, will
    // have the integer value 0 and b) using a pointer-to-data-member with a
    // null pointer constant for which a sufficiently smart compiler might warn
    // about.
    const ptrdiff_t link_offset =
        reinterpret_cast<ptrdiff_t>(
            &(reinterpret_cast<const T*>(0)->*link_ptr));

    // Now we construct our fake T* by subtracting the link offset from the
    // start address of "this".
    return reinterpret_cast<T*>(
      const_cast<char*>(reinterpret_cast<const char*>(this) - link_offset));
  }

  // Transfers the contents of one intrusive_list to another.
  static void transfer(intrusive_list<T, link_ptr> *src,
                       intrusive_list<T, link_ptr> *dest) {
    for (typename intrusive_list<T, link_ptr>::iterator i = src->begin();
         i != src->end();) {
      typename intrusive_list<T, link_ptr>::iterator next_i = i;
      ++next_i;
      T *entry = &*i;
      src->erase(entry);
      dest->push_back(entry);
      i = next_i;
    }
  }

 public:
  typedef T value_type;
  typedef value_type *pointer;
  typedef const value_type *const_pointer;
  typedef value_type &reference;
  typedef const value_type &const_reference;
  typedef size_t size_type;
  typedef ptrdiff_t difference_type;
  typedef intrusive_list<T, link_ptr> self;
  typedef intrusive_list_iterator<T*, T&, T*> iterator;
  typedef intrusive_list_iterator<const T*, const T&, const T*>
    const_iterator;
  typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
  typedef std::reverse_iterator<iterator> reverse_iterator;

 public:
  // Constructor.
  intrusive_list() {
    clear();
  }

  // Iterator routines.
  iterator begin() { return iterator(link_.next(end_node()), end_node()); }
  const_iterator begin() const {
    return const_iterator(link_.next(end_node()), end_node()); }
  iterator end() { return iterator(end_node(), end_node()); }
  const_iterator end() const { return const_iterator(end_node(), end_node()); }
  reverse_iterator rbegin() { return reverse_iterator(end()); }
  const_reverse_iterator rbegin() const {
    return const_reverse_iterator(end());
  }
  reverse_iterator rend() { return reverse_iterator(begin()); }
  const_reverse_iterator rend() const {
    return const_reverse_iterator(begin());
  }

  reverse_iterator reverse_iterator_from(T *node) {
    return reverse_iterator(iterator_from(node));
  }

  iterator iterator_from(T *node) {
    return iterator(node, end_node());
  }
  // Size routines.
  bool empty() const { return (link_.next(end_node()) == end_node()); }
  size_type size() const { return std::distance(begin(), end()); }
  size_type max_size() const { return size_type(-1); }

  // Front and back accessors.
  reference front() { return *link_.next(end_node()); }
  const_reference front() const { return *link_.next(end_node()); }
  reference back() { return *link_.prev(end_node()); }
  const_reference back() const { return *link_.prev(end_node()); }

  // Insertion routines.
  iterator insert(T *position, T *obj) {
    T *next = position;
    if (next == NULL) {
      return iterator();
    }
    T *prev = (position->*link_ptr).prev(end_node());
    if (prev == NULL) {
      return iterator();
    }
    (next->*link_ptr).set_prev(end_node(), obj);
    (obj->*link_ptr).set_next(end_node(), next);
    (obj->*link_ptr).set_prev(end_node(), prev);
    (prev->*link_ptr).set_next(end_node(), obj);
    return iterator(obj, end_node());
  }

  iterator insert(iterator position, T *obj) {
    return insert(position.node_, obj);
  }

  void push_front(T *obj) { insert(link_.next(end_node()), obj); }
  void push_back(T *obj) {
    insert(end_node(), obj);
  }

  // Removal routines.
  iterator erase(T *obj) {
    // Fix up the next and previous links for the previous and next objects.
    T *next = (obj->*link_ptr).next(end_node());
    if (next == NULL) {
      return iterator();
    }
    T *prev = (obj->*link_ptr).prev(end_node());
    if (prev == NULL) {
      return iterator();
    }
    (next->*link_ptr).set_prev(end_node(), prev);
    (prev->*link_ptr).set_next(end_node(), next);
    (obj->*link_ptr).reset();
    return iterator(next, end_node());
  }
  iterator erase(iterator position) {
    return erase(position.node);
  }
  void pop_front() { erase(link_.next(end_node())); }
  void pop_back() { erase(link_.prev(end_node())); }

  // Utility routines.
  void clear() {
    T *end = end_node();
    link_.reset();
    link_.set_next(end_node(), end);
    link_.set_prev(end_node(), end);
  }
  void swap(intrusive_list<T, link_ptr> &x) {
    intrusive_list<T, link_ptr> tmp;
    transfer(this, &tmp);
    transfer(&x, this);
    transfer(&tmp, &x);
  }

 private:
  intrusive_link<T> link_;
};
#endif  // APP_QZAP_COMMON_UTILITY_INTRUSIVE_LIST_H_
