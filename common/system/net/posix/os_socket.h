// Copyright (c) 2011, Tencent Inc. All rights reserved.
// Author: CHEN Feng <phongchen@tencent.com>

#ifndef COMMON_SYSTEM_NET_POSIX_OS_SOCKET_H_
#define COMMON_SYSTEM_NET_POSIX_OS_SOCKET_H_

#include <errno.h>

#include <fcntl.h>
#include <poll.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <net/if.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>

#include <string>

namespace gdt {

const int INVALID_SOCKET_HANDLE = -1;
const int SOCKET_ERROR_RETURN = -1;

inline int SocketGetLastError() {
  return errno;
}

inline void SocketSetLastError(int error) {
  errno = error;
}

inline std::string SocketGetErrorString(int error) {
  return strerror(error);
}

inline int closesocket(int s) {
  return close(s);
}

inline int ioctlsocket(int s, int cmd, void* p) {
  return ioctl(s, cmd, p);
}

inline int SocketShutdown(int s) {
  return shutdown(s, SHUT_RDWR);
}

inline int SocketShutdownSend(int s) {
  return shutdown(s, SHUT_WR);
}

inline int SocketShutdownReceive(int s) {
  return shutdown(s, SHUT_RD);
}

inline int SocketSetNonblocking(int s, bool value) {
  int flags = fcntl(s, F_GETFL, 0);
  if (flags < 0)
    return flags;
  if (value)
    flags |= O_NONBLOCK;
  else
    flags &= ~O_NONBLOCK;
  return fcntl(s, F_SETFL, flags);
}

inline int SocketGetNonblocking(int s, bool* value) {
  int flags = fcntl(s, F_GETFL, 0);
  if (flags < 0)
    return flags;
  *value = (flags & O_NONBLOCK) != 0;
  return 0;
}

inline int Socket_PollReadable(int fd, struct timeval* tv) {
  int timeout = tv ? (tv->tv_sec * 1000 + tv->tv_usec / 1000) : -1;
  pollfd pfd;
  pfd.fd = fd;
  pfd.events = POLLIN;
  return poll(&pfd, 1, timeout);
}

inline int Socket_PollWriteable(int fd, struct timeval* tv) {
  int timeout = tv ? (tv->tv_sec * 1000 + tv->tv_usec / 1000) : -1;
  pollfd pfd;
  pfd.fd = fd;
  pfd.events = POLLOUT;
  return poll(&pfd, 1, timeout);
}

}  // namespace gdt

#define SOCKET_ERROR_CODE(e) e

#endif  // COMMON_SYSTEM_NET_POSIX_OS_SOCKET_H_
