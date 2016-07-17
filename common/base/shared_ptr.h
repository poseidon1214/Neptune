// Copyright    (C), 1998-2012, Tencent
// Author: jefftang@tencent.com
// Date: 2012-11-22
// The implement for the shared_ptr.

#ifndef COMMON_BASE_SHARED_PTR_H_
#define COMMON_BASE_SHARED_PTR_H_

#include <tr1/functional>  // For hash
#include <tr1/memory>
#include <functional>

// We don't put shared_ptr into namespace gdt because it is a c++11 standard
// component.
// Import to default namespace to make old code compatible and easy upgrade to
// c++11.
using std::tr1::shared_ptr;
using std::tr1::weak_ptr;
using std::tr1::enable_shared_from_this;
using std::tr1::bad_weak_ptr;
using std::tr1::static_pointer_cast;
using std::tr1::dynamic_pointer_cast;
using std::tr1::const_pointer_cast;

// Needed by unordered containers, but is missing in gcc 4.1.2
namespace std {
namespace tr1 {
template<typename T> struct hash<shared_ptr<T> > {
  size_t operator()(const shared_ptr<T> &x) const {
    return hash<const T*>()(x.get());
  }
};
}  // namespace tr1
template <typename T>
struct equal_to<shared_ptr<T> > {
  bool operator()(const shared_ptr<T> &t1,
                  const shared_ptr<T> &t2) const {
    return t1.get() == t2.get();
  }
};
}  // namespace std

#endif  // COMMON_BASE_SHARED_PTR_H_
