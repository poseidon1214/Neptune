// Copyright (c) 2011, Tencent Inc.
// All rights reserved.
//
// Author: CHEN Feng <phongchen@tencent.com>
// Created: 08/04/11
// Description:

#ifndef COMMON_BASE_STRING_COMPARE_H
#define COMMON_BASE_STRING_COMPARE_H
#pragma once

#include <stddef.h>
#include <string>

#include "common/base/static_assert.h"
#include "common/base/string/string_piece.h"
#include "app/qzap/common/recordio/internal/unaligned.h"

#include <ctype.h>
#include <limits.h>
#include <string.h>
#include <stdint.h>

// namespace common {

inline bool memeql_2(const char* p1, const char* p2) {
  return *(uint16_t*)&p1[0] == *(uint16_t*)&p2[0];
}

inline bool memeql_3(const char* p1, const char* p2) {
  return
    *(uint16_t*)&p1[0] == *(uint16_t*)&p2[0] &&
    p1[2] == p2[2];
}

inline bool memeql_4(const char* p1, const char* p2) {
  return
    *(uint32_t*)&p1[0] == *(uint32_t*)&p2[0];
}

inline bool memeql_5(const char* p1, const char* p2) {
  return
    *(uint32_t*)&p1[0] == *(uint32_t*)&p2[0] &&
    p1[4] == p2[4];
}

inline bool memeql_6(const char* p1, const char* p2) {
  return
    *(uint32_t*)&p1[0] == *(uint32_t*)&p2[0] &&
    *(uint16_t*)&p1[4] == *(uint16_t*)&p2[4];
}

inline bool memeql_7(const char* p1, const char* p2) {
  return
    *(uint32_t*)&p1[0] == *(uint32_t*)&p2[0] &&
    *(uint16_t*)&p1[4] == *(uint16_t*)&p2[4] &&
    p1[6] == p2[6];
}

inline bool memeql_8(const char* p1, const char* p2) {
  return *(uint64_t*)&p1[0] == *(uint64_t*)&p2[0];
}

inline bool memeql_9(const char* p1, const char* p2) {
  return
    *(uint64_t*)&p1[0] == *(uint64_t*)&p2[0] &&
    p1[8] == p2[8];
}

inline bool memeql_10(const char* p1, const char* p2) {
  return
    *(uint64_t*)&p1[0] == *(uint64_t*)&p2[0] &&
    *(uint16_t*)&p1[8] == *(uint16_t*)&p2[8];
}

inline bool memeql_11(const char* p1, const char* p2) {
  return
    *(uint64_t*)&p1[0] == *(uint64_t*)&p2[0] &&
    *(uint16_t*)&p1[8] == *(uint16_t*)&p2[8] &&
    p1[10] == p2[10];
}

inline bool memeql_12(const char* p1, const char* p2) {
  return
    *(uint64_t*)&p1[0] == *(uint64_t*)&p2[0] &&
    *(uint32_t*)&p1[8] == *(uint32_t*)&p2[8];
}

inline bool memeql_13(const char* p1, const char* p2) {
  return
    *(uint64_t*)&p1[0] == *(uint64_t*)&p2[0] &&
    *(uint32_t*)&p1[8] == *(uint32_t*)&p2[8] &&
    p1[12] == p2[12];
}

inline bool memeql_14(const char* p1, const char* p2) {
  return
    *(uint64_t*)&p1[0] == *(uint64_t*)&p2[0] &&
    *(uint32_t*)&p1[8] == *(uint32_t*)&p2[8] &&
    *(uint16_t*)&p1[12] == *(uint16_t*)&p2[12];
}

inline bool memeql_15(const char* p1, const char* p2) {
  return
    *(uint64_t*)&p1[0] == *(uint64_t*)&p2[0] &&
    *(uint32_t*)&p1[8] == *(uint32_t*)&p2[8] &&
    *(uint16_t*)&p1[12] == *(uint16_t*)&p2[12] &&
    p1[14] == p2[14];
}

inline bool memeql_16(const char* p1, const char* p2) {
  return
    *(uint64_t*)&p1[0] == *(uint64_t*)&p2[0] &&
    *(uint64_t*)&p1[8] == *(uint64_t*)&p2[8];
}

// An optimized fast memory compare function, should be inlined
inline
bool memeql(const void* a1, const void* a2, size_t size) {
#if ALIGNMENT_INSENSITIVE_PLATFORM
  // optimize for alignment insensitive architectures
  const char* p1 = (const char*)a1;
  const char* p2 = (const char*)a2;

  switch (size) {
  case 0:
    return true;
  case 1:
    return p1[0] == p2[0];
  case 2:
    return memeql_2(p1, p2);
  case 3:
    return memeql_3(p1, p2);
  case 4:
    return memeql_4(p1, p2);
  case 5:
    return memeql_5(p1, p2);
  case 6:
    return memeql_6(p1, p2);
  case 7:
    return memeql_7(p1, p2);
  case 8:
    return memeql_8(p1, p2);
  case 9:
    return memeql_9(p1, p2);
  case 10:
    return memeql_10(p1, p2);
  case 11:
    return memeql_11(p1, p2);
  case 12:
    return memeql_12(p1, p2);
  case 13:
    return memeql_13(p1, p2);
  case 14:
    return memeql_14(p1, p2);
  case 15:
    return memeql_15(p1, p2);
  case 16:
    return memeql_16(p1, p2);
  }

  while (size >= 8) {
    if (*(uint64_t*)&p1[0] != *(uint64_t*)&p2[0])
      return false;
    p1 += 8;
    p2 += 8;
    size -= 8;
  }
  if (size >= 4) {
    if (*(uint32_t*)&p1[0] != *(uint32_t*)&p2[0])
      return false;
    p1 += 4;
    p2 += 4;
    size -= 4;
  }
  if (size >= 2) {
    if (*(uint16_t*)&p1[0] != *(uint16_t*)&p2[0])
      return false;
    p1 += 2;
    p2 += 2;
    size -= 2;
  }
  if (size == 1)
    return p1[0] == p2[0];

  return true;
#else
  return memcmp(a1, a2, size) == 0;
#endif
}

#ifndef __GNUC__
inline const void* internal_memmem(
  const void *haystack,
  size_t haystack_len,
  const void *needle,
  size_t needle_len
) {
  const char *begin;
  const char *const last_possible
    = (const char *) haystack + haystack_len - needle_len;

  if (needle_len == 0) {
    /* The first occurrence of the empty string is deemed to occur at
       the beginning of the string.  */
    return CONST_CAST(void *, haystack);
  }

  /* Sanity check, otherwise the loop might search through the whole
     memory.  */
  if (haystack_len < needle_len)
    return NULL;

  for (begin = (const char *) haystack; begin <= last_possible; ++begin) {
    if (begin[0] == ((const char *) needle)[0] &&
        memeql(&begin[1], ((const char *) needle + 1), needle_len - 1))
      return begin;
  }

  return NULL;
}

#ifdef __cplusplus
inline const void* memmem(
  const void *haystack,
  size_t haystack_len,
  const void *needle,
  size_t needle_len
) {
  return internal_memmem(haystack, haystack_len, needle, needle_len);
}
inline void* memmem(
  void *haystack,
  size_t haystack_len,
  const void *needle,
  size_t needle_len
) {
  return CONST_CAST(void*, internal_memmem(haystack, haystack_len, needle,
                    needle_len));
}
#else
inline void* memmem(
  const void *haystack,
  size_t haystack_len,
  const void *needle,
  size_t needle_len
) {
  return CONST_CAST(void*, internal_memmem(haystack, haystack_len, needle,
                    needle_len));
}
#endif

#endif

// token from linux kernel
/**
 * strlcpy - Copy a %NUL terminated string into a sized buffer
 * @param dest Where to copy the string to
 * @param src Where to copy the string from
 * @param size size of destination buffer
 * @return the total length of the string tried to create
 *
 * Compatible with *BSD: the result is always a valid
 * NUL-terminated string that fits in the buffer (unless,
 * of course, the buffer size is zero). It does not pad
 * out the result like strncpy() does.
 */
inline size_t strlcpy(char *dest, const char *src, size_t size) {
  size_t ret = strlen(src);

  if (size) {
    size_t len = (ret >= size) ? size - 1 : ret;
    memcpy(dest, src, len);
    dest[len] = '\0';
  }
  return ret;
}

/**
 * strlcat - Append a length-limited, %NUL-terminated string to another
 * @param dest The string to be appended to
 * @param src The string to append to it
 * @param count The size of the destination buffer.
 * @return the total length of the string tried to create
 */
inline size_t strlcat(char *dest, const char *src, size_t count) {
  size_t dsize = strlen(dest);
  size_t len = strlen(src);
  size_t res = dsize + len;

  if (dsize >= count) {
    dest += dsize;
    count -= dsize;
    if (len >= count)
      len = count - 1;
    memcpy(dest, src, len);
    dest[len] = '\0';
  }
  return res;
}

#ifdef _MSC_VER

inline
const void* internal_memrchr(const void* start, int c, size_t len) {
  const char* end = STATIC_CAST(const char*, start) + len;

  while (--end, len--) {
    if (*end == STATIC_CAST(char, c))
      return end;
  }

  return NULL;
}
#ifdef __cplusplus
inline const void* memrchr(const void* start, int c, size_t len) {
  return internal_memrchr(start, c, len);
}
inline void* memrchr(void* start, int c, size_t len) {
  return const_cast<void*>(internal_memrchr(start, c, len));
}
#else
inline void* memrchr(const void* start, int c, size_t len) {
  return CONST_CAST(void*, internal_memrchr(start, c, len));
}
#endif

inline int strcasecmp(const char *s1, const char *s2) {
  return _stricmp(s1, s2);
}

inline
int strncasecmp(const char *s1, const char *s2, size_t len) {
  return _strnicmp(s1, s2, len);
}

inline int strerror_r(int errnum, char *buf, size_t buflen) {
  return strerror_s(buf, buflen, errnum);
}

#endif

inline
int memcasecmp(const void *vs1, const void *vs2, size_t n) {
  size_t i;
  const unsigned char *s1 =
    static_cast<const unsigned char*>(vs1);
  const unsigned char *s2 =
    static_cast<const unsigned char*>(vs2);
  for (i = 0; i < n; i++) {
    unsigned char u1 = s1[i];
    unsigned char u2 = s2[i];
    int U1 = tolower(u1);
    int U2 = tolower(u2);
    int diff = (UCHAR_MAX <= INT_MAX ? U1 - U2
                : U1 < U2 ? -1 : U2 < U1);
    if (diff)
      return diff;
  }
  return 0;
}

/// search a string for any of a set of characters
/// @param str string to search
/// @param chars set of characters to be found
/// @return a pointer to the character in s that matches one of the characters in accept
/// @retval NULL no such character is found.
/// @sa strpbrk
inline const char* strnpbrk(const char* str, size_t length, const char* chars) {
  unsigned char bitmap[UCHAR_MAX / CHAR_BIT] = {};
  const unsigned char* p = (const unsigned char*)chars;
  while (*p) {
    bitmap[*p / CHAR_BIT] |= 1U << (*p % CHAR_BIT);
    ++chars;
  }

  size_t i;
  for (i = 0; i < length; ++i) {
    if (bitmap[(unsigned char)str[i] / CHAR_BIT] & (1U << (str[i] % CHAR_BIT)))
      return str + i;
  }
  return NULL;
}


// An optimized fast memory compare function, should be inlined
inline bool MemoryEqual(const void* a1, const void* a2, size_t size) {
  return memeql(a1, a2, size);
}

inline int CompareMemory(const void *b1, const void *b2, size_t len,
                         size_t* prefix_length) {
  STATIC_ASSERT(sizeof(size_t) == 8 || sizeof(size_t) == 4 ||
                sizeof(size_t) == 2);

  const unsigned char * const a = (const unsigned char *)b1;
  const unsigned char * const b = (const unsigned char *)b2;

  // pos must bu signed type
  ptrdiff_t pos = 0;
  ptrdiff_t end_pos = len - sizeof(size_t);

  int result = 0;

#define COMPARE_MEMORY_ONE_BYTE() \
    result = a[pos] - b[pos]; \
    if (result) { \
        *prefix_length = pos;\
        return result;\
    } \
    ++pos

  while (pos <= end_pos) { // compare by word size
    if (GetUnaligned<size_t>(a + pos) != GetUnaligned<size_t>(b + pos)) {
      switch (sizeof(size_t)) {
      case 8:
        COMPARE_MEMORY_ONE_BYTE();
        COMPARE_MEMORY_ONE_BYTE();
        COMPARE_MEMORY_ONE_BYTE();
        COMPARE_MEMORY_ONE_BYTE();
      // fall through
      case 4:
        COMPARE_MEMORY_ONE_BYTE();
        COMPARE_MEMORY_ONE_BYTE();
      // fall through
      case 2:
        COMPARE_MEMORY_ONE_BYTE();
        COMPARE_MEMORY_ONE_BYTE();
      }
      assert(!"unreachable");
    }
    pos += sizeof(size_t);
  }

  switch (len - pos) { // handle tail
  case 7:
    COMPARE_MEMORY_ONE_BYTE();
  case 6:
    COMPARE_MEMORY_ONE_BYTE();
  case 5:
    COMPARE_MEMORY_ONE_BYTE();
  case 4:
    COMPARE_MEMORY_ONE_BYTE();
  case 3:
    COMPARE_MEMORY_ONE_BYTE();
  case 2:
    COMPARE_MEMORY_ONE_BYTE();
  case 1:
    COMPARE_MEMORY_ONE_BYTE();
  }

#undef COMPARE_MEMORY_ONE_BYTE

  *prefix_length = len;
  return result; // match
}

inline int CompareMemory(const void *b1, const void *b2, size_t len) {
  size_t prefix_length;
  return CompareMemory(b1, b2, len, &prefix_length);
}

/// @brief  求两个串的最大公共前缀串
/// @param  lhs     lhs的buffer
/// @param  lhs_len lhs的长度
/// @param  rhs     rhs的buffer
/// @param  rhs_len rhs的长度
/// @return 最大公共前缀串的长度
inline size_t GetCommonPrefixLength(
  const void* lhs, size_t lhs_len,
  const void* rhs, size_t rhs_len
) {
  size_t prefix_length;
  size_t common_length = lhs_len < rhs_len ? lhs_len : rhs_len;
  CompareMemory(lhs, rhs, common_length, &prefix_length);
  return prefix_length;
}

inline size_t GetCommonPrefixLength(const std::string& lhs,
                                    const std::string& rhs) {
  return GetCommonPrefixLength(lhs.c_str(), lhs.length(),
                               rhs.c_str(), rhs.length());
}

/// @brief  按字节大小比较字符串lhs 和 rhs
/// @param  lhs     lhs的buffer
/// @param  lhs_len lhs的长度
/// @param  rhs     rhs的buffer
/// @param  rhs_len rhs的长度
/// @param  inclusive 返回两个字符串是否存在包含关系
/// @retval <0 lhs < rhs
/// @retval 0  lhs = rhs;
/// @retval >0 lhs > rhs
/// @note 需要 inline
inline int CompareByteString(const void* lhs, size_t lhs_len,
                             const void* rhs, size_t rhs_len, bool* inclusive,
                             size_t* common_prefix_len = NULL) {
  const unsigned char* p1 = reinterpret_cast<const unsigned char*>(lhs);
  const unsigned char* p2 = reinterpret_cast<const unsigned char*>(rhs);
  ptrdiff_t min_len = (lhs_len <= rhs_len) ? lhs_len : rhs_len;
  ptrdiff_t pos = 0;
  ptrdiff_t end_pos = min_len - sizeof(size_t) + 1;

  while (pos < end_pos) {
    if (GetUnaligned<size_t>(p1 + pos) == GetUnaligned<size_t>(p2 + pos))
      pos += sizeof(size_t); // 按机器字长剔除公共前缀串
    else
      break;
  }

  while ((pos < min_len) && (p1[pos] == p2[pos]))
    pos++;

  *inclusive = (pos == min_len);

  if (common_prefix_len != NULL)
    *common_prefix_len = pos;

  if (*inclusive) {
    if (lhs_len > rhs_len)
      return 1;
    else if (lhs_len == rhs_len)
      return 0;
    else
      return -1;
  } else {
    return p1[pos] - p2[pos];
  }
}

/// @brief  按字节大小比较字符串lhs 和 rhs
/// @param  lhs     lhs的buffer
/// @param  lhs_len lhs的长度
/// @param  rhs     rhs的buffer
/// @param  rhs_len rhs的长度
/// @retval <0 lhs < rhs
/// @retval 0  lhs = rhs;
/// @retval >0 lhs > rhs
inline int CompareByteString(
  const void* lhs, size_t lhs_len,
  const void* rhs, size_t rhs_len
) {
  bool inclusive;
  return CompareByteString(lhs, lhs_len, rhs, rhs_len, &inclusive);
}

inline int CompareByteString(const std::string& lhs, const std::string& rhs) {
  return CompareByteString(lhs.c_str(), lhs.length(), rhs.c_str(), rhs.length());
}

inline int CompareStringIgnoreCase(const StringPiece& lhs,
                                   const StringPiece& rhs) {
  return lhs.ignore_case_compare(rhs);
}

inline bool StringEqualsIgnoreCase(const StringPiece& lhs,
                                   const StringPiece& rhs) {
  return lhs.ignore_case_equal(rhs);
}

// } // namespace common

#endif // COMMON_BASE_STRING_COMPARE_H
