// Copyright    (C), 1998-2012, Tencent
// Author: jefftang@tencent.com
// Date: 2012-11-22
#ifndef APP_QZAP_COMMON_BASE_BASE_H_
#define APP_QZAP_COMMON_BASE_BASE_H_
#include <stdio.h>
#include <tr1/unordered_map>
#include <tr1/unordered_set>
template <class T, int N>
inline int arraysize(T (&x)[N]) {
  return N;
}

// A macro to disallow the copy constructor and operator= functions
// This should be used in the private: declarations for a class
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&);               \
  void operator=(const TypeName&)

// The COMPILE_ASSERT macro can be used to verify that a compile time
// expression is true. For example, you could use it to verify the
// size of a static array:
//
//   COMPILE_ASSERT(ARRAYSIZE(content_type_names) == CONTENT_NUM_TYPES,
//                  content_type_names_incorrect_size);
//
// or to make sure a struct is smaller than a certain size:
//
//   COMPILE_ASSERT(sizeof(foo) < 128, foo_too_large);
//
// The second argument to the macro is the name of the variable. If
// the expression is false, most compilers will issue a warning/error
// containing the name of the variable.

template <bool>
struct GdtCompileAssert {
};

#define COMPILE_ASSERT(expr, msg) \
  typedef GdtCompileAssert<(static_cast<bool>(expr))> \
  msg[static_cast<bool>(expr) ? 1 : -1]

// Use implicit_cast as a safe version of static_cast or const_cast
// for upcasting in the type hierarchy (i.e. casting a pointer to Foo
// to a pointer to SuperclassOfFoo or casting a pointer to Foo to
// a const pointer to Foo).
// When you use implicit_cast, the compiler checks that the cast is safe.
// Such explicit implicit_casts are necessary in surprisingly many
// situations where C++ demands an exact type match instead of an
// argument type convertable to a target type.
//
// The From type can be inferred, so the preferred syntax for using
// implicit_cast is the same as for static_cast etc.:
//
//   implicit_cast<ToType>(expr)
//
// implicit_cast would have been part of the C++ standard library,
// but the proposal was submitted too late.  It will probably make
// its way into the language in the future.
template<typename To, typename From>
inline To implicit_cast(From const &f) {
  return f;
}

/**
 * container_of - cast a member of a structure out to the containing structure
 * @ptr:	the pointer to the member.
 * @type:	the type of the container struct this is embedded in.
 * @member:	the name of the member within the struct.
 *
 */
#undef container_of
#ifdef __cplusplus
#define container_of(ptr, type, member) ({ \
  typedef type container_type; \
  typedef decltype((reinterpret_cast<container_type*>(0))->member) member_type; \
  char *__mptr = reinterpret_cast<char*>(ptr); \
  member_type container_type::*ln = &container_type::member;\
  const size_t _m_offset = reinterpret_cast<size_t>(&\
  (reinterpret_cast<container_type*>(0)->*ln)); \
  reinterpret_cast<container_type *>(__mptr - _m_offset);})
#else
#define container_of(ptr, type, member) ({ \
  const decltype((reinterpret_cast<type *>(0))->member) *__mptr = (ptr);\
  reinterpret_cast<type *>(reinterpret_cast<char *>(__mptr) -\
                           offsetof(type, member));})
#endif
#endif  // APP_QZAP_COMMON_UTILITY_BASE_H_
