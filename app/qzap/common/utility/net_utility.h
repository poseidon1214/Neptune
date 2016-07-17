// Copyright (C), 1998-2012, Tencent
// Author: jefftang@tencent.com
// Date: 2013-2-26
#ifndef APP_QZAP_COMMON_UTILITY_NET_UTILITY_H_
#define APP_QZAP_COMMON_UTILITY_NET_UTILITY_H_

#include <stdint.h>
#include <string>
#include <vector>

std::string GetIpAddressByInterfaceName(const std::string &interface_name);
bool Resolve(const std::string &host, std::vector<struct in_addr> *addresses);
int Connect(const std::string &host, uint16_t port);

bool IsAvaliablePort(uint16_t port);
bool IsAvaliablePort(uint16_t port, int type);

uint16_t PickAvailablePort();
uint16_t PickAvailablePort(int type);

#endif  // APP_QZAP_COMMON_UTILITY_NET_UTILITY_H_
