// Copyright (C), 1998-2012, Tencent
// Author: jefftang@tencent.com
// Date: 2013-2-26
#include <stdio.h>
#include <stdlib.h>
#include <string.h> /* for strncpy */
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>
#include "app/qzap/common/utility/net_utility.h"
#include "app/qzap/common/base/string_utility.h"
#include "app/qzap/common/base/scoped_ptr.h"
#include "thirdparty/glog/logging.h"

static pthread_once_t once_control = PTHREAD_ONCE_INIT;
static void RandomSetSeedOnce() {
  srand(time(NULL));
}

std::string GetIpAddressByInterfaceName(const std::string &interface_name) {
  int fd;
  struct ifreq ifr;

  fd = socket(AF_INET, SOCK_DGRAM, 0);

  /* I want to get an IPv4 IP address */
  ifr.ifr_addr.sa_family = AF_INET;

  /* I want IP address attached to "eth0" */
  strncpy(ifr.ifr_name, interface_name.c_str(), IFNAMSIZ-1);

  ioctl(fd, SIOCGIFADDR, &ifr);

  close(fd);
  return StringPrintf(
      "%s", inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
}

bool Resolve(
  const std::string &host,
  std::vector<struct in_addr> *addresses) {
  static const int kBufferSize = 8192;
  int herrno = 0;
  struct hostent hostinfo;
  struct hostent* result;
  scoped_array<char> buffer(new char[kBufferSize]);
  if (0 != gethostbyname_r(
        host.c_str(), &hostinfo, buffer.get(), kBufferSize,
        &result, &herrno)) {
    return false;
  }

  for (int i = 0; hostinfo.h_addr_list[i]; ++i) {
    struct in_addr addr = *reinterpret_cast<struct in_addr*>(
        hostinfo.h_addr_list[i]);
    addresses->push_back(addr);
  }
  return !addresses->empty();
}

static bool InternalConnect(int fd, const socklen_t &socklen,
                            const std::string &host,
                            uint16_t port,
                            struct sockaddr_in* addr) {
  if (connect(fd, (struct sockaddr*)addr, socklen) < 0) {
    LOG(WARNING) << "Fail to connect to "
      << host << "::" << port <<  " error:"
      << strerror(errno);
    if (errno != EINTR) {
      return false;
    }
  }

  // handle errno = EINTR case
  struct pollfd fds[1];
  fds[0].fd = fd;
  fds[0].events = POLLOUT;
  int ret = poll(fds, 1, 1000);
  if (ret > 0) {
    int val;
    socklen_t lon;
    lon = sizeof(val);
    ret = getsockopt(fd, SOL_SOCKET, SO_ERROR,
                     static_cast<void *>(&val), &lon);
    if (ret == -1) {
      LOG(WARNING) << "Fail to connect to "
        << host << "::" << port <<  " getsockopt() error:"
        << strerror(errno);
      return false;
    }
    if (val == 0) {
      return true;
    }
    LOG(WARNING) << "Fail to connect to "
      << host << "::" << port <<  " error:" << val;
    return false;
  } else if (ret == 0) {
    LOG(WARNING) << "Fail to connect to "
      << host << "::" << port <<  " poll() time out";
    return false;
  } else {
    LOG(WARNING) << "Fail to connect to "
      << host << "::" << port <<  " poll() failed";
    return false;
  }
}

int Connect(const std::string &host, uint16_t port) {
  int fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd == -1) {
    LOG(ERROR) << "Fail to create socket fd"
      <<  " error:" << strerror(errno);
    return -1;
  }
  std::vector<in_addr>  addresses;
  if (host == "0.0.0.0") {
    in_addr addr;
    addr.s_addr = INADDR_ANY;
    addresses.push_back(addr);
  } else {
    if (!Resolve(host, &addresses)) {
      close(fd);
      return -1;
    }
  }
  struct sockaddr_in addr;
  socklen_t socklen = sizeof(addr);
  size_t size = addresses.size();
  for (size_t i = 0; i < size; ++i) {
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr = addresses[i];
    if (!InternalConnect(fd, socklen, host, port, &addr)) {
      // try to connect to another address
      continue;
    }
    return fd;
  }
  close(fd);
  return -1;
}

bool IsAvaliablePort(uint16_t port, int type) {
  int sock = socket(PF_INET, type, 0);
  if (sock < 0) {
    return false;
  }
  int reuse_flag = 1;
  setsockopt(sock, SOL_SOCKET, SO_REUSEADDR,
             &reuse_flag, sizeof(reuse_flag));
  sockaddr_in sin;
  sin.sin_family = AF_INET;
  sin.sin_addr.s_addr = INADDR_ANY;
  sin.sin_port = htons(port);
  bool ret;
  if (bind(sock, (struct sockaddr *)&sin, sizeof(sin)) != 0) {
    VLOG(2) << "bind error port: " << port;
    ret = false;
  } else {
    ret = true;
  }
  close(sock);
  return ret;
}

bool IsAvaliablePort(uint16_t port) {
  return IsAvaliablePort(port, SOCK_STREAM);
}

uint16_t PickAvailablePort(int type) {
  pthread_once(&once_control, RandomSetSeedOnce);
  uint16_t port;
  do {
    port = ((rand() % (65535 - 1024))  // NOLINT(runtime/threadsafe_fn)
            + 1024);
  } while (!IsAvaliablePort(port, type));
  return port;
}

uint16_t PickAvailablePort() {
  return PickAvailablePort(SOCK_STREAM);
}

