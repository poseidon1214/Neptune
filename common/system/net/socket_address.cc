// Copyright (c) 2011, Tencent Inc.
// All rights reserved.
//
// Author: CHEN Feng <phongchen@tencent.com>
// Created: 09/30/11
// Description: sockaddr wrapper

#include "common/system/net/socket_address.h"
#include <string>
#include "thirdparty/glog/logging.h"

namespace gdt {

static bool ParseSocketAddress4(const char* str, sockaddr_in* target) {
  int b[4];
  int port;
  char dummy;  // catch extra character
  int count = sscanf(str, "%i.%i.%i.%i:%d%c",
                     &b[0], &b[1], &b[2], &b[3], &port, &dummy);
  if (count != 5)
    return false;
  for (int i = 0; i < 4; ++i) {
    if (b[i] < 0 || b[i] > UCHAR_MAX)
      return false;
  }
  if (port < 0 || port > USHRT_MAX)
    return false;

  memset(target, 0, sizeof(*target));
  unsigned char* p = reinterpret_cast<unsigned char*>(&target->sin_addr);
  for (int i = 0; i < 4; ++i) {
    p[i] = b[i];
  }
  target->sin_port = htons(port);
  target->sin_family = AF_INET;
  return true;
}

static bool ParseSocketAddress6(const char* str, sockaddr_in6* target) {
  char text[INET6_ADDRSTRLEN];
  int port;
  char dummy;  // catch extra character
  int count = sscanf(str, "[%[^]]]:%d%c",
                     text, &port, &dummy);
  if (count != 2)
    return false;
  if (port < 0 || port > USHRT_MAX)
    return false;
  in6_addr in6a;
  if (inet_pton(AF_INET6, text, &in6a) <= 0)
    return false;
  memset(target, 0, sizeof(*target));
  target->sin6_addr = in6a;
  target->sin6_port = htons(port);
  target->sin6_family = AF_INET6;
  return true;
}

static void SocketAddressToString(const sockaddr_in* address,
                                  std::string* str) {
  char text[INET_ADDRSTRLEN + sizeof(":65536")];
  const unsigned char* p =
      reinterpret_cast<const unsigned char*>(&address->sin_addr);
  int length = snprintf(text, sizeof(text), "%u.%u.%u.%u:%d",
                        p[0], p[1], p[2], p[3], ntohs(address->sin_port));
  str->assign(text, length);
}

static void SocketAddressToString(const sockaddr_in6* address,
                                  std::string* str) {
  char text[INET6_ADDRSTRLEN];
  if (inet_ntop(AF_INET6, &address->sin6_addr, text, sizeof(text))) {
    str->push_back('[');
    str->append(text);
    str->push_back(']');
    snprintf(text, sizeof(text), ":%d", ntohs(address->sin6_port));
    str->append(text);
  }
}

static void SocketAddressToString(const sockaddr_un* address,
                                  std::string* str) {
  if (address->sun_path[0] == '\0' && address->sun_path[1] != '\0') {
    *str = '@';
    str->append(address->sun_path + 1);
  } else {
    str->assign(address->sun_path);
  }
}

/// Abstract sockaddr

SocketAddressInet4::SocketAddressInet4(const char* src) {
  CHECK(Assign(src)) << "Invalid IPv4 socket address: " << src;
}

SocketAddressInet4::SocketAddressInet4(const std::string& src) {
  CHECK(Assign(src)) << "Invalid IPv4 socket address: " << src;
}

SocketAddressInet4::SocketAddressInet4(const char* src, uint16_t port) {
  CHECK(Assign(src, port)) << "Invalid IPv4 socket address: " << src;
}

SocketAddressInet4::SocketAddressInet4(const std::string& src, uint16_t port) {
  CHECK(Assign(src, port)) << "Invalid IPv4 socket address: " << src;
}

SocketAddressInet4& SocketAddressInet4::operator=(const SocketAddress& rhs) {
  CHECK(CopyFrom(rhs)) << "SocketAddress: Can't copy from " << rhs.ToString();
  return *this;
}

bool SocketAddressInet4::Assign(uint32_t ip, uint16_t port) {
  m_address.sin_addr.s_addr = ip;
  m_address.sin_port = htons(port);
  return true;
}

bool SocketAddressInet4::Assign(const IPAddress& ip, uint16_t port) {
  m_address.sin_addr = ip.ToInAddr();
  m_address.sin_port = htons(port);
  return true;
}

bool SocketAddressInet4::Assign(
    unsigned char b1,
    unsigned char b2,
    unsigned char b3,
    unsigned char b4,
    uint16_t port) {
  unsigned char* p = reinterpret_cast<unsigned char*>(&m_address.sin_addr);
  p[0] = b1;
  p[1] = b2;
  p[2] = b3;
  p[3] = b4;

  m_address.sin_port = htons(port);

  return true;
}

bool SocketAddressInet4::Assign(const char* str) {
  return ParseSocketAddress4(str, &m_address);
}

bool SocketAddressInet4::Assign(const char* str, uint16_t port) {
  IPAddress ip;
  if (ip.Assign(str))
    return Assign(ip, port);
  return false;
}

int SocketAddressInet4::Compare(const SocketAddressInet4& rhs) const {
  if (GetIP() > rhs.GetIP())
    return 1;
  else if (GetIP() < rhs.GetIP())
    return -1;

  return GetPort() - rhs.GetPort();
}

void SocketAddressInet4::DoToString(std::string* str) const {
  SocketAddressToString(&m_address, str);
}

/////////////////////////////////////////////////////////////////////////////
// Socket address encapsulation for IPv6

SocketAddressInet6::SocketAddressInet6(const char* src) {
  CHECK(Parse(src)) << "Invalid IPv6 socket address: " << src;
}

SocketAddressInet6::SocketAddressInet6(const std::string& src) {
  CHECK(Parse(src)) << "Invalid IPv6 socket address: " << src;
}

void SocketAddressInet6::DoToString(std::string* str) const {
  SocketAddressToString(&m_address, str);
}

bool SocketAddressInet6::DoParse(const char* str) {
  return false;
}

/////////////////////////////////////////////////////////////////////////////
// SocketAddressInet members

SocketAddressInet::SocketAddressInet(const char* src) : m_address() {
  CHECK(Parse(src)) << "Invalid IPv4/6 socket address: " << src;
}

SocketAddressInet::SocketAddressInet(const std::string& src) : m_address() {
  CHECK(Parse(src)) << "Invalid IPv4/6 socket address: " << src;
}

SocketAddressInet::SocketAddressInet(uint32_t ip, uint16_t port)
  : m_address() {
  m_address.v4.sin_family = AF_INET;
  m_address.v4.sin_addr.s_addr = ip;
  m_address.v4.sin_port = htons(port);
}

SocketAddressInet::SocketAddressInet(
  unsigned char b1,
  unsigned char b2,
  unsigned char b3,
  unsigned char b4,
  uint16_t port) :
  m_address() {
  m_address.v4.sin_family = AF_INET;

  unsigned char* p = reinterpret_cast<unsigned char*>(&m_address.v4.sin_addr);
  p[0] = b1;
  p[1] = b2;
  p[2] = b3;
  p[3] = b4;

  m_address.v4.sin_port = htons(port);
}

SocketAddress& SocketAddress::operator=(const SocketAddress& rhs) {
  CHECK(CopyFrom(rhs)) << "SocketAddress: Can't copy from " << rhs.ToString();
  return *this;
}

socklen_t SocketAddressInet::Length() const {
  switch (m_address.common.sa_family) {
  case AF_INET:
    return sizeof(m_address.v4);
  case AF_INET6:
    return sizeof(m_address.v6);
  default:
    assert(false);
  }
  return 0;
}

bool SocketAddressInet::SetLength(socklen_t length) {
  switch (m_address.common.sa_family) {
  case AF_INET:
    return length == sizeof(m_address.v4);
  case AF_INET6:
    return length == sizeof(m_address.v6);
  default:
    assert(false);
  }
  return false;
}

SocketAddressInet& SocketAddressInet::operator=(const SocketAddress& src) {
  CHECK(src.Family() == AF_INET || src.Family() == AF_INET6)
      << "SocketAddress: Can't copy from " << src.ToString();
  memcpy(Address(), src.Address(), src.Length());
  return *this;
}

void SocketAddressInet::DoToString(std::string* str) const {
  switch (m_address.common.sa_family) {
  case AF_INET:
    SocketAddressToString(&m_address.v4, str);
    break;
  case AF_INET6:
    SocketAddressToString(&m_address.v6, str);
    break;
  default:
    // return empty string if not a valid IP4/6 socket address
    break;
  }
}

bool SocketAddressInet::DoParse(const char* str) {
  if (ParseSocketAddress4(str, &m_address.v4))
    return true;
  if (ParseSocketAddress6(str, &m_address.v6))
    return true;
  return false;
}

bool SocketAddressInet::DoCopyFrom(const SocketAddress& rhs) {
  switch (rhs.Family()) {
  case AF_INET:
  case AF_INET6:
    memcpy(&m_address, rhs.Address(), rhs.Length());
    return true;
  default:
    return false;
  }
}

uint16_t SocketAddressInet::GetPort() const {
  switch (m_address.common.sa_family) {
  case AF_INET:
    return ntohs(m_address.v4.sin_port);
  case AF_INET6:
    return ntohs(m_address.v6.sin6_port);
  }
  assert(!"Invalid address family of SocketAddressInet");
  return 0;
}

bool SocketAddressInet::Equals(const SocketAddressInet& rhs) const {
  if (m_address.common.sa_family != rhs.m_address.common.sa_family)
    return false;
  switch (m_address.common.sa_family) {
    case AF_INET:
      return memcmp(&m_address.v4, &rhs.m_address.v4,
                    sizeof(m_address.v4)) == 0;
    case AF_INET6:
      return memcmp(&m_address.v6, &rhs.m_address.v6,
                    sizeof(m_address.v6)) == 0;
    case AF_UNSPEC:
      return true;
  }
  assert(!"Invalid address family of SocketAddressInet");
  return false;
}

/////////////////////////////////////////////////////////////////////////////
// Unix domain socket address
SocketAddressUnix::~SocketAddressUnix() {}

SocketAddressUnix::SocketAddressUnix(const char* name) {
  CHECK(Parse(name)) << "Invalid unix domain socket address: " << name;
}

SocketAddressUnix::SocketAddressUnix(const std::string& name) {
  CHECK(Parse(name)) << "Invalid unix domain socket address: " << name;
}

bool SocketAddressUnix::DoParse(const char* name) {
  // "/data/local.socket"
  // "@/data/local.socket"
  // check invalid path
  if (name[0] == '/' || (name[0] == '@' && name[1] == '/')) {
    size_t length = strlen(name);
    if (length + 1 > sizeof(m_address.sun_path))
      return false;
    memset(m_address.sun_path, 0, sizeof(m_address.sun_path));
    memcpy(m_address.sun_path, name, length + 1);
    if (m_address.sun_path[0] == '@') {
      m_address.sun_path[0] = '\0';
    }
    return true;
  }
  return false;
}

socklen_t SocketAddressUnix::Length() const {
  socklen_t fixed_part_length = offsetof(struct sockaddr_un, sun_path);
  if (m_address.sun_path[0] == '\0' && m_address.sun_path[1] != '\0') {
    return fixed_part_length + strlen(m_address.sun_path + 1) + 1;
  }
  return fixed_part_length + strlen(m_address.sun_path);
}

void SocketAddressUnix::DoToString(std::string* str) const {
  SocketAddressToString(&m_address, str);
}

// for store any type socket address
SocketAddressStorage::~SocketAddressStorage() {}

void SocketAddressStorage::DoToString(std::string* str) const {
  switch (m_address.ss_family) {
  case AF_UNIX: {
      const sockaddr_un* ua = reinterpret_cast<const sockaddr_un*>(&m_address);
      SocketAddressToString(ua, str);
    }
    break;
  case AF_INET: {
      const sockaddr_in* saddrin =
        reinterpret_cast<const sockaddr_in*>(&m_address);
      SocketAddressToString(saddrin, str);
    }
    break;
  case AF_INET6: {
      const sockaddr_in6* saddrin =
        reinterpret_cast<const sockaddr_in6*>(&m_address);
      SocketAddressToString(saddrin, str);
    }
    break;
  default:
    break;
  }
}

bool SocketAddressStorage::DoParse(const char* str) {
  // try inet address
  {
    SocketAddressInet address;
    if (address.Parse(str)) {
      *this = address;
      return true;
    }
  }
  // try unix domain address
  {
    SocketAddressUnix address;
    if (address.Parse(str)) {
      *this = address;
      return true;
    }
  }
  return false;
}

bool SocketAddressStorage::DoCopyFrom(const SocketAddress& rhs) {
  m_address.ss_family = rhs.Family();
  memcpy(this->Address(), rhs.Address(), rhs.Length());
  return SetLength(rhs.Length());
}

}  // namespace gdt
