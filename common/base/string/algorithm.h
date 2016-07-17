// Copyright (c) 2011, Tencent Inc. All rights reserved.

/// @brief string algorithms
/// @author hsiaokangliu
/// @date 2010-11-26

#ifndef COMMON_BASE_STRING_ALGORITHM_H_
#define COMMON_BASE_STRING_ALGORITHM_H_

#include <limits.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>

#include <set>
#include <string>
#include <vector>

#include "common/base/string/string_piece.h"
#include "common/encoding/ascii.h"

// Compare has been moved to compare.h, include here to keep compatible
#include "common/base/string/compare.h"
#include "common/base/string/string_number.h"

// namespace common {

inline bool IsWhiteString(char const *str) {
  if (!str) return false;

  size_t i = 0;
  size_t len = strlen(str);
  while (i < len && Ascii::IsSpace(str[i])) {
    i++;
  }
  return (i == len);
}

inline bool IsCharInString(char c, const StringPiece& str) {
  size_t len = str.size();
  for (size_t i = 0; i < len; i++) {
    if (str[i] == c) return true;
  }
  return false;
}

inline bool StringStartsWith(const StringPiece& str,
                             const StringPiece& prefix) {
  return str.starts_with(prefix);
}

std::string HexDump(const std::string &s);
// Lower-level routine that takes a va_list and appends to a specified
// string.  All other routines are just convenience wrappers around it.
void StringAppendV(std::string* dst, const char* format, va_list ap);

// Return a C++ string
std::string StringPrintf(const char* format, ...)
  __attribute__((format(printf, 1, 2)));

// Store result into a supplied string and return it
const std::string& SStringPrintf(std::string* dst, const char* format, ...)
  __attribute__((format(printf, 2, 3)));

// Append result to a supplied string
void StringAppendF(std::string* dst, const char* format, ...)
  __attribute__((format(printf, 2, 3)));

// gcc std::string == is quit slow.
static inline bool StringEqual(const std::string &s1, const std::string &s2) {
  return s1.size() == s2.size() && MemoryEqual(s1.data(), s2.data(), s1.size());
}
bool StringStartsWithIgnoreCase(const StringPiece& str,
                                const StringPiece& prefix);

inline std::string StringRemovePrefix(const StringPiece& str,
                                      const StringPiece& prefix) {
  if (StringStartsWith(str, prefix)) {
    return str.substr(prefix.size()).as_string();
  }
  return str.as_string();
}

inline bool StringRemovePrefix(std::string* str, const StringPiece& prefix) {
  if (StringStartsWith(*str, prefix)) {
    str->erase(0, prefix.size());
    return true;
  }
  return false;
}

inline bool StringEndsWith(const StringPiece& str, const StringPiece& suffix) {
  return str.ends_with(suffix);
}

inline std::string StringRemoveSuffix(const StringPiece& str,
                                      const StringPiece& suffix) {
  if (StringEndsWith(str, suffix)) {
    return str.substr(0, str.size() - suffix.size()).as_string();
  }
  return str.as_string();
}

inline bool StringRemoveSuffix(std::string* str, const StringPiece& suffix) {
  if (StringEndsWith(*str, suffix)) {
    str->erase(str->size() - suffix.size(), suffix.size());
    return true;
  }
  return false;
}

inline bool ContainsSubString(const StringPiece& str, const StringPiece& sub) {
  return str.find(sub) != StringPiece::npos;
}

std::string ReplaceFirst(const StringPiece& s, const StringPiece& from,
                         const StringPiece& to);
void ReplaceFirst(std::string* s,
                  const StringPiece& from,
                  const StringPiece& to);

std::string ReplaceAll(const StringPiece& s, const StringPiece& from,
                       const StringPiece& to);
void ReplaceAll(std::string* s, const StringPiece& from, const StringPiece& to);

// replace recursively, ex.StringReplace("thhe", "th", "t") will get "te"
void StringReplace(std::string *str,const std::string &from,
                   const std::string &to);
// replace not recursively, just the same as ReplaceAll
// ex.StringReplaceNonRec("thhe", "th", "t") will get "the"
void StringReplaceNonRec(std::string *str, const std::string &from,
                         const std::string &to);



size_t ReplaceAllChars(std::string* s, const StringPiece& from, char to);
std::string ReplaceAllChars(const StringPiece& s, const StringPiece& from,
                            char to);

std::string StripString(const StringPiece& s, const char* remove,
                        char replacewith);
inline std::string StripString(const StringPiece& s, const std::string& remove,
                               char replacewith) {
  return StripString(s, remove.c_str(), replacewith);
}

void StripString(std::string* s, const char* remove, char replacewith);
inline void StripString(std::string* s, const std::string& remove,
                        char replacewith) {
  return StripString(s, remove.c_str(), replacewith);
}

char* RemoveLineEnding(char* line);
void RemoveLineEnding(std::string* line);
void RemoveLineEnding(StringPiece* line);

std::string RemoveContinuousBlank(const StringPiece& str);
void RemoveContinuousBlank(std::string* str);

//    These methods concatenate a vector of strings into a C++ string, using
//    the C-string "delim" as a separator between components.
//
//    There are two flavors of the function, one flavor returns the concatenated string,
//    another takes a pointer to the target string. In the latter case the
//    target string is cleared and overwritten.
//
//    There are also interfaces for STL-like iterator range, the typical usage is to join
//    repeated string field in protocol buffer message.
//    See also: thirdparty/protobuf/repeated_field.h
std::string JoinStrings(const std::vector<std::string>& components,
                        const StringPiece& delim);
void JoinStrings(const std::vector<std::string>& components,
                 const StringPiece& delim,
                 std::string* res);

template <class InputIterator>
void JoinStrings(InputIterator begin_iter,
                 InputIterator end_iter,
                 const StringPiece& delim,
                 std::string* result) {
  if (begin_iter == end_iter) {
    return;
  }
  *result = *begin_iter++;
  for (InputIterator iter = begin_iter; iter != end_iter; ++iter) {
    result->append(delim.data(), delim.length());
    result->append(iter->data(), iter->size());
  }
}

template <class InputIterator>
std::string JoinStrings(InputIterator begin_iter, InputIterator end_iter,
                        const StringPiece& delim) {
  std::string result;
  JoinStrings<InputIterator>(begin_iter, end_iter, delim, &result);
  return result;
}
// TODO(naicaisun): use JoinStrings instead
template <typename I>
static inline std::string JoinString(I begin, I end,
                                     const std::string &seperator) {
  return JoinStrings(begin, end, seperator);
}
// TODO(naicaisun): use JoinStrings instead
static inline std::string JoinString(const std::vector<std::string> &field,
                                     const std::string &seperator) {
  return JoinStrings(field.begin(), field.end(), seperator);
}

// split by any character in separators, just like SplitByAnyOf
// yet SplitString will get {"a","","b",""} given input like
// SplitString("a|=b|", "|="), while SplitByAnyOf will get {"a","b"}
// TODO(naicaisun): change name as SplitStringByAnyOfKeepEmpty
bool SplitString(const std::string &src,
                 const char *separators,
                 std::vector<std::string> *result);

// Split a string using a character delimiter. fill the components into
// 'result'.  If there are consecutive delimiters, this function skips over all
// of them.
// Note: For multi-character delimiters, this routine will split on *ANY* of
// the characters in the string, not the entire string as a single delimiter.
// So it's NOT the reverse function of JoinStrings.
void SplitStringByAnyOf(const StringPiece& full, const char* delim,
                        std::vector<std::string>* res);

// The 'delim' is a delimiter string, it's the reverse function of JoinStrings.
void SplitString(const StringPiece& full,
                 const char* delim,
                 std::vector<std::string>* result);

void SplitString(const StringPiece& full,
                 const char* delim,
                 std::vector<StringPiece>* result);

// The 'delim' is a delimiter string, it's the reverse function of JoinStrings.
void SplitStringAndStrip(const StringPiece& full,
                         const char* delim,
                         std::vector<std::string>* result);

void SplitStringToSet(const StringPiece& full,
                      const char* delim,
                      std::set<std::string>* result);

void SplitStringKeepEmpty(
  const StringPiece& full,
  char delim,
  std::vector<std::string>* result
);

void SplitStringKeepEmpty(
  const StringPiece& full,
  const StringPiece& delim,
  std::vector<std::string>* result
);

// TODO(naicaisun): delete it or change function name to 'SplitStringNotStrip'
bool SplitStringByString(const std::string &src,
    const std::string &separator,
    std::vector<std::string> *result);

template<typename T>
bool SplitNumericString(const std::string& numeric_string,
                        const char* separators,
                        std::vector<T>* numbers) {
  std::vector<std::string> segs;
  if (!SplitString(numeric_string, separators, &segs)) {
    return false;
  }
  for (size_t i = 0; i < segs.size(); ++i) {
    if (segs[i].empty()) {
      continue;
    }
    T number;
    if (!StringToNumeric(segs[i], &number)) {
      return false;
    }
    numbers->push_back(number);
  }
  return true;
}

void SplitLines(
  const StringPiece& full,
  std::vector<std::string>* result,
  bool keep_line_endling = false
);

void SplitLines(
  const StringPiece& full,
  std::vector<StringPiece>* result,
  bool keep_line_endling = false
);

/////////////////////////////////////////////////////////////////////////////
// Return stripped value

void LTrimString(std::string *str);
void RTrimString(std::string *str);
void TrimString(std::string *str);

std::string StringTrimLeft(const StringPiece& str);
std::string StringTrimLeft(const StringPiece& str,
                           const StringPiece& trim_chars);
inline std::string StringTrimLeft(const StringPiece& str, char trim_char) {
  return StringTrimLeft(str, StringPiece(&trim_char, 1));
}

std::string StringTrimRight(const StringPiece& str);
std::string StringTrimRight(const StringPiece& str,
                            const StringPiece& trim_chars);
inline std::string StringTrimRight(const StringPiece& str, char trim_char) {
  return StringTrimRight(str, StringPiece(&trim_char, 1));
}

std::string StringTrim(const StringPiece& str);
std::string StringTrim(const StringPiece& str, const StringPiece& trim_chars);
inline std::string StringTrim(const StringPiece& str, char trim_char) {
  return StringTrim(str, StringPiece(&trim_char, 1));
}

/////////////////////////////////////////////////////////////////////////////
// Strip string inplace

void StringTrimLeft(std::string* str);
void StringTrimLeft(std::string* str, const StringPiece& trim_chars);
inline void StringTrimLeft(std::string* str, char trim_char) {
  StringTrimLeft(str, StringPiece(&trim_char, 1));
}

void StringTrimRight(std::string* str);
void StringTrimRight(std::string* str, const StringPiece& trim_chars);
inline void StringTrimRight(std::string* str, char trim_char) {
  StringTrimRight(str, StringPiece(&trim_char, 1));
}

void StringTrim(std::string* str);
void StringTrim(StringPiece* str);

void StringTrim(std::string* str, const StringPiece& trim_chars);
void StringTrim(StringPiece* str, const StringPiece& trim_chars);
inline void StringTrim(std::string* str, char trim_char) {
  StringTrim(str, StringPiece(&trim_char, 1));
}

std::string RemoveFirst(
  const StringPiece& str,
  const StringPiece& sub,
  bool fill_blank = false
);

std::string RemoveAll(
  const StringPiece& str,
  const StringPiece& sub,
  bool fill_blank = false);


int CEscapeString(const char* src, int src_len, char* dest, int dest_len);
std::string CEscapeString(const StringPiece& src);

/// Make sure the dest buffer is big enough
int CUnescapeString(const char* src, char* dest);

std::string CUnescapeString(const std::string& src);

inline void StringToUpper(std::string* s) {
  std::string::iterator end = s->end();
  for (std::string::iterator i = s->begin(); i != end; ++i)
    *i = toupper(static_cast<unsigned char>(*i));
}

inline void StringToLower(std::string* s) {
  std::string::iterator end = s->end();
  for (std::string::iterator i = s->begin(); i != end; ++i)
    *i = tolower(static_cast<unsigned char>(*i));
}

inline std::string UpperString(const StringPiece& s) {
  std::string res = s.as_string();
  StringToUpper(&res);
  return res;
}

// inline std::string LowerString(const StringPiece& s)
// {
//     std::string res = s.as_string();
//     StringToLower(&res);
//     return res;
// }

// } // namespace common

#endif  // COMMON_BASE_STRING_ALGORITHM_H_
