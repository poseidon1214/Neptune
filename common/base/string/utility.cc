// Copyright (c) 2015 Tencent Inc.

#include "common/base/string/utility.h"

#include <wordexp.h>

#include "thirdparty/glog/logging.h"
#include "thirdparty/openssl/md5.h"
#include "thirdparty/zlib/zlib.h"

#include "app/qzap/common/base/base.h"  // arraysize

bool PopenToString(const char *command, std::string *result) {
  CHECK(command != NULL);
  CHECK(result != NULL);
  result->clear();
  FILE*fp = popen(command, "r");
  if (!fp) {
    return false;
  }
  char buffer[200];
  while (fgets(buffer, sizeof(buffer), fp)) {
    *result += buffer;
  }
  pclose(fp);
  return true;
}

bool PopenFromString(const char *command, const char *input) {
  CHECK(command != NULL);
  CHECK(input != NULL);
  FILE *fp = popen(command, "w");
  if (!fp) {
    return false;
  }
  fprintf(fp, "%s", input);
  pclose(fp);
  return true;
}

uint32_t ChecksumString(const std::string& str) {
  uLong adler = adler32(0L, Z_NULL, 0);
  return adler32(
    adler, reinterpret_cast<const Bytef *>(str.c_str()), str.length());
}

std::string MD5sumString(const std::string &str) {
  static const char kLookupHex[] = "0123456789abcdef";
  unsigned char md[MD5_DIGEST_LENGTH];
  MD5(reinterpret_cast<const unsigned char*>(str.c_str()),
      str.size(), reinterpret_cast<unsigned char*>(&md[0]));
  std::string ret;
  ret.resize(arraysize(md) * 2);
  for (int i = 0; i < arraysize(md); ++i) {
    const int lo = md[i] & 0xF;
    const int hi = (md[i] >> 4) & 0xF;
    ret[2 * i] = kLookupHex[hi];
    ret[2 * i + 1] = kLookupHex[lo];
  }
  return ret;
}

bool StringExp(const char *exp, std::vector<std::string> *result) {
  wordexp_t p;
  char **w;
  if (0 != wordexp(exp, &p, 0)) {
    LOG(ERROR) << "word fail, error: " << errno << " : " << strerror(errno);
    return false;
  }
  w = p.we_wordv;
  for (uint32_t i = 0; i < p.we_wordc; i++) {
    result->push_back(std::string(w[i]));
  }
  wordfree(&p);
  return true;
}

