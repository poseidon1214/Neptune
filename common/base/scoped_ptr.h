// Copyright    (C), 1998-2012, Tencent
// Author: jefftang@tencent.com
// Date: 2012-11-22

#ifndef COMMON_BASE_SCOPED_PTR_H_
#define COMMON_BASE_SCOPED_PTR_H_

#include <assert.h>
#include <stddef.h>
#include <tr1/type_traits>
#include <algorithm>

namespace gdt {

template <class C, typename D> class scoped_ptr;
template <class C> class scoped_array;
template <typename T> struct default_delete;

template <class C>
scoped_ptr<C, default_delete<C> > make_scoped_ptr(C* p);

// Primary template, default_delete.
template <typename T>
struct default_delete {
  void operator()(T* ptr) const {
    enum { type_must_be_complete = sizeof(T) };
    delete ptr;
  }
};

// Primary template for common stated Deleter class.
template <
  typename Type,
  typename Deleter,
  bool DeleterIsEmpty>
class scoped_ptr_base_impl {
 protected:
  scoped_ptr_base_impl(Type* ptr, Deleter deleter)
    : ptr_(ptr), deleter_(deleter) {
  }

  void swap(scoped_ptr_base_impl& rhs) {
    std::swap(ptr_, rhs.ptr_);
    std::swap(deleter_, rhs.deleter_);
  }

 public:
  // Get stored deleter
  Deleter& get_deleter() {
    return deleter_;
  }

  // Get stored deleter
  const Deleter& get_deleter() const {
    return deleter_;
  }

 protected:
  Type* ptr_;        // Stored pointer
  Deleter deleter_;  // Stored deleter
};

// Specialization for empty Deleter class, ensure no space overhead.
template <
  typename Type,
  typename Deleter>
class scoped_ptr_base_impl<Type, Deleter, true> : protected Deleter {
  // Inherit from deleter to enable empty base class optimization.
 protected:
  scoped_ptr_base_impl(Type* ptr, Deleter deleter)
    : Deleter(deleter), ptr_(ptr) {
  }

  void swap(scoped_ptr_base_impl& rhs) {
    std::swap(ptr_, rhs.ptr_);
  }

 public:
  // Get stored deleter
  Deleter& get_deleter() {
    return *this;
  }

  // Get stored deleter
  const Deleter& get_deleter() const {
    return *this;
  }

 protected:
  Type* ptr_;        // Stored pointer
};

// A scoped_ptr<T> is like a T*, except that the destructor of scoped_ptr<T>
// automatically deletes the pointer it holds (if any).
// That is, scoped_ptr<T> owns the T object that it points to.
// Like a T*, a scoped_ptr<T> may hold either NULL or a pointer to a T object.
// Also like T*, scoped_ptr<T> is thread-compatible, and once you
// dereference it, you get the threadsafety guarantees of T.
//
// The size of a scoped_ptr is small:
// sizeof(scoped_ptr<C>) == sizeof(C*)
template <class C, typename D = default_delete<C> >
class scoped_ptr :
  public scoped_ptr_base_impl<C, D, std::tr1::is_empty<D>::value> {
  typedef scoped_ptr_base_impl<C, D, std::tr1::is_empty<D>::value> base;
  typedef C* scoped_ptr::*unspecified_bool_type;

 public:
  // The element type
  typedef C element_type;

  // Constructor.  Defaults to intializing with NULL.
  // There is no way to create an uninitialized scoped_ptr.
  // The input parameter must be allocated with new.
  explicit scoped_ptr(C* p = NULL, D d = D()) : base(p, d) { }

  // Destructor.  If there is a C object, delete it.
  // We don't need to test ptr_ == NULL because C++ does that for us.
  ~scoped_ptr() {
    enum { type_must_be_complete = sizeof(C) };
    this->get_deleter()(this->ptr_);
  }

  // Reset.  Deletes the current owned object, if any.
  // Then takes ownership of a new object, if given.
  // this->reset(this->get()) works.
  void reset(C* p = NULL) {
    if (p != this->ptr_) {
      enum { type_must_be_complete = sizeof(C) };
      this->get_deleter()(this->ptr_);
      this->ptr_ = p;
    }
  }

  // Accessors to get the owned object.
  // operator* and operator-> will assert() if there is no current object.
  C& operator*() const {
    assert(this->ptr_ != NULL);
    return *this->ptr_;
  }
  C* operator->() const  {
    assert(this->ptr_ != NULL);
    return this->ptr_;
  }
  C* get() const { return this->ptr_; }

  // Comparison operators.
  // These return whether a scoped_ptr and a raw pointer refer to
  // the same object, not just to two different but equal objects.
  bool operator==(const C* p) const { return this->ptr_ == p; }
  bool operator!=(const C* p) const { return this->ptr_ != p; }

  // Allow scoped_ptr<element_type> to be used in boolean expressions, but not
  // implicitly convertible to a real bool (which is dangerous).
  operator unspecified_bool_type() const {
    return !this->ptr_ ? NULL : &scoped_ptr::ptr_;
  }

  // Swap two scoped pointers.
  void swap(scoped_ptr& p2) {
    base::swap(p2);
  }

  // Release a pointer.
  // The return value is the current pointer held by this object.
  // If this object holds a NULL pointer, the return value is NULL.
  // After this operation, this object will hold a NULL pointer,
  // and will not own the object any more.
  C* release() {
    C* retVal = this->ptr_;
    this->ptr_ = NULL;
    return retVal;
  }

 private:
  // google3 friend class that can access copy ctor (although if it actually
  // calls a copy ctor, there will be a problem) see below
  friend scoped_ptr<C> make_scoped_ptr<C>(C *p);

  // Forbid comparison of scoped_ptr types.  If C2 != C, it totally doesn't
  // make sense, and if C2 == C, it still doesn't make sense because you should
  // never have the same object owned by two different scoped_ptrs.
  template <class C2> bool operator==(scoped_ptr<C2> const& p2) const;
  template <class C2> bool operator!=(scoped_ptr<C2> const& p2) const;

  // Disallow evil constructors
  scoped_ptr(const scoped_ptr&);
  scoped_ptr& operator=(const scoped_ptr&);
};

// Free functions
template <class C, typename D>
inline void swap(scoped_ptr<C, D>& p1, scoped_ptr<C, D>& p2) {
  p1.swap(p2);
}

template <class C, typename D>
inline bool operator==(const C* p1, const scoped_ptr<C, D>& p2) {
  return p1 == p2.get();
}

template <class C, typename D>
inline bool operator==(const C* p1, const scoped_ptr<const C, D>& p2) {
  return p1 == p2.get();
}

template <class C, typename D>
inline bool operator!=(const C* p1, const scoped_ptr<C, D>& p2) {
  return p1 != p2.get();
}

template <class C, typename D>
inline bool operator!=(const C* p1, const scoped_ptr<const C, D>& p2) {
  return p1 != p2.get();
}

template <class C>
scoped_ptr<C> make_scoped_ptr(C *p) {
  // This does nothing but to return a scoped_ptr of the type that the passed
  // pointer is of.  (This eliminates the need to specify the name of T when
  // making a scoped_ptr that is used anonymously/temporarily.)  From an
  // access control point of view, we construct an unnamed scoped_ptr here
  // which we return and thus copy-construct.  Hence, we need to have access
  // to scoped_ptr::scoped_ptr(scoped_ptr const &).  However, it is guaranteed
  // that we never actually call the copy constructor, which is a good thing
  // as we would call the temporary's object destructor (and thus delete p)
  // if we actually did copy some object, here.
  return scoped_ptr<C>(p);
}

// scoped_array<C> is like scoped_ptr<C>, except that the caller must allocate
// with new [] and the destructor deletes objects with delete [].
//
// As with scoped_ptr<C>, a scoped_array<C> either points to an object
// or is NULL.  A scoped_array<C> owns the object that it points to.
// scoped_array<T> is thread-compatible, and once you index into it,
// the returned objects have only the threadsafety guarantees of T.
//
// Size: sizeof(scoped_array<C>) == sizeof(C*)
template <class C>
class scoped_array {
  typedef C* scoped_array::*unspecified_bool_type;

 public:
  // The element type
  typedef C element_type;

  // Constructor.  Defaults to intializing with NULL.
  // There is no way to create an uninitialized scoped_array.
  // The input parameter must be allocated with new [].
  explicit scoped_array(C* p = NULL) : array_(p) { }

  // Destructor.  If there is a C object, delete it.
  // We don't need to test ptr_ == NULL because C++ does that for us.
  ~scoped_array() {
    enum { type_must_be_complete = sizeof(C) };
    delete[] array_;
  }

  // Reset.  Deletes the current owned object, if any.
  // Then takes ownership of a new object, if given.
  // this->reset(this->get()) works.
  void reset(C* p = NULL) {
    if (p != array_) {
      enum { type_must_be_complete = sizeof(C) };
      delete[] array_;
      array_ = p;
    }
  }

  // Get one element of the current object.
  // Will assert() if there is no current object, or index i is negative.
  C& operator[](std::ptrdiff_t i) const {
    assert(i >= 0);
    assert(array_ != NULL);
    return array_[i];
  }

  // Get a pointer to the zeroth element of the current object.
  // If there is no current object, return NULL.
  C* get() const {
    return array_;
  }

  // Comparison operators.
  // These return whether a scoped_array and a raw pointer refer to
  // the same array, not just to two different but equal arrays.
  bool operator==(const C* p) const { return array_ == p; }
  bool operator!=(const C* p) const { return array_ != p; }

  // Allow scoped_ptr<element_type> to be used in boolean expressions, but not
  // implicitly convertible to a real bool (which is dangerous).
  operator unspecified_bool_type() const {
    return !this->array_ ? NULL : &scoped_array::array_;
  }

  // Swap two scoped arrays.
  void swap(scoped_array& p2) {
    C* tmp = array_;
    array_ = p2.array_;
    p2.array_ = tmp;
  }

  // Release an array.
  // The return value is the current pointer held by this object.
  // If this object holds a NULL pointer, the return value is NULL.
  // After this operation, this object will hold a NULL pointer,
  // and will not own the object any more.
  C* release() {
    C* retVal = array_;
    array_ = NULL;
    return retVal;
  }

 private:
  C* array_;

  // Forbid comparison of different scoped_array types.
  template <class C2> bool operator==(scoped_array<C2> const& p2) const;
  template <class C2> bool operator!=(scoped_array<C2> const& p2) const;

  // Disallow evil constructors
  scoped_array(const scoped_array&);
  void operator=(const scoped_array&);
};

// Free functions
template <class C>
inline void swap(scoped_array<C>& p1, scoped_array<C>& p2) {
  p1.swap(p2);
}

template <class C>
inline bool operator==(const C* p1, const scoped_array<C>& p2) {
  return p1 == p2.get();
}

template <class C>
inline bool operator==(const C* p1, const scoped_array<const C>& p2) {
  return p1 == p2.get();
}

template <class C>
inline bool operator!=(const C* p1, const scoped_array<C>& p2) {
  return p1 != p2.get();
}

template <class C>
inline bool operator!=(const C* p1, const scoped_array<const C>& p2) {
  return p1 != p2.get();
}

}  // namespace gdt

#endif  // COMMON_BASE_SCOPED_PTR_H_
