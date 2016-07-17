// Copyright (c) 2011, Tencent Inc.
// All rights reserved.
//
// Author: CHEN Feng <phongchen@tencent.com>
// Created: 09/30/11
// Description: IPAddress implementation

#include "common/system/net/ip_address.h"
#include <net/if.h>

namespace gdt {

IPAddress::IPAddress(const char* src) {
  if (!Assign(src))
    throw std::runtime_error(std::string("Invalid IP Address: ") + src);
}

IPAddress::IPAddress(const std::string& src) {
  if (!Assign(src))
    throw std::runtime_error("Invalid IP Address: " + src);
}

bool IPAddress::Assign(const char* src) {
  int bytes[4];
  char dummy; // catch extra character
  int count = sscanf(src, "%i.%i.%i.%i%c",
                     &bytes[0], &bytes[1], &bytes[2], &bytes[3], &dummy);
  if (count != 4)
    return false;

  for (int i = 0; i < 4; ++i) {
    if (bytes[i] < 0 || bytes[i] > UCHAR_MAX)
      return false;
  }

  Assign((unsigned char)bytes[0], (unsigned char)bytes[1],
         (unsigned char)bytes[2], (unsigned char)bytes[3]);
  return true;
}

void IPAddress::ToString(std::string* str) const {
  char text[INET_ADDRSTRLEN];
  int length = snprintf(text, INET_ADDRSTRLEN, "%u.%u.%u.%u",
                        m_bytes[0], m_bytes[1], m_bytes[2], m_bytes[3]);
  str->assign(text, length);
}

#ifdef _WIN32

bool IPAddress::GetLocalList(std::vector<IPAddress>* v) {
  WSADATA wsaData;
  if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
    return false;
  }

  char hostname[128];
  if (gethostname(hostname, sizeof(hostname)) == 0) {
    struct hostent* host = gethostbyname(hostname);
    if (host) {
      int i = -1;
      do {
        i++;
        v->push_back(IPAddress(*reinterpret_cast<IN_ADDR*>
                               (host->h_addr_list[i])));
      } while (host->h_addr_list[i] + host->h_length < host->h_name);
    }
  }
  WSACleanup();
  return true;
}

#else

bool IPAddress::GetLocalList(std::vector<IPAddress>* v) {
  v->clear();

  int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0) {
    return false;
  }

  // 初始化ifconf
  struct ifconf ifconf;
  char buffer[512];
  ifconf.ifc_len = 512;
  ifconf.ifc_buf = buffer;

  // 获取所有接口信息
  ioctl(sockfd, SIOCGIFCONF, &ifconf);
  close(sockfd);

  struct ifreq *ifreq = reinterpret_cast<struct ifreq*>(buffer);
  for (size_t i = 0; i < (ifconf.ifc_len / sizeof(struct ifreq)); ++i) {
    IPAddress ip(((struct sockaddr_in*) & (ifreq->ifr_addr))->sin_addr);
    if (strcmp(ifreq->ifr_name, "lo") != 0 && !ip.IsLoopback()) {
      v->push_back(ip);
    }
    ifreq++;
  }
  return true;
}

#endif

bool IPAddress::GetPrivateList(std::vector<IPAddress>* v) {
  if (!GetLocalList(v))
    return false;

  std::vector<IPAddress>::iterator iter = v->begin();
  while (iter != v->end()) {
    if (!iter->IsPrivate()) {
      iter = v->erase(iter);
    } else {
      ++iter;
    }
  }
  if (v->empty()) {
    return false;
  }
  return true;
}

bool IPAddress::GetPublicList(std::vector<IPAddress>* v) {
  if (!GetLocalList(v))
    return false;

  std::vector<IPAddress>::iterator iter = v->begin();
  while (iter != v->end()) {
    if (!iter->IsPublic()) {
      iter = v->erase(iter);
    } else {
      ++iter;
    }
  }
  if (v->empty()) {
    return false;
  }
  return true;
}

bool IPAddress::GetFirstLocalAddress(IPAddress* a) {
  std::vector<IPAddress> v;
  if (GetLocalList(&v)) {
    *a = v[0];
    return true;
  }
  return false;
}

bool IPAddress::GetFirstPrivateAddress(IPAddress* a) {
  std::vector<IPAddress> v;
  if (GetPrivateList(&v)) {
    *a = v[0];
    return true;
  }
  return false;
}

bool IPAddress::GetFirstPublicAddress(IPAddress* a) {
  std::vector<IPAddress> v;
  if (GetPublicList(&v)) {
    *a = v[0];
    return true;
  }
  return false;
}

} // namespace common

