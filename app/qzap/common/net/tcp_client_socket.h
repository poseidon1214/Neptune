// Copyright (C), 1998-2013, Tencent
// Author: neilchen@tencent.com
// Date: 2013-07-04
#ifndef APP_QZAP_COMMON_NET_TCP_CLIENT_SOCKET_H_
#define APP_QZAP_COMMON_NET_TCP_CLIENT_SOCKET_H_
#include <stdint.h>
#include <string>
class TCPClientSocket
{
    public:
        TCPClientSocket(const std::string &ip, uint16_t port);
        TCPClientSocket(const std::string &ip, uint16_t port, int32_t conn_timeout_ms, 
                int32_t send_timeout_ms, int32_t recv_timeout_ms);
        ~TCPClientSocket();
        int32_t Connect(const int32_t *timeout_ms=NULL);
        int32_t Send(const void *buffer, int32_t len, const int32_t *timeout_ms=NULL);
        int32_t Recv(void *buffer, int32_t len, const int32_t *timeout_ms=NULL);
        int32_t RecvOnce(void *buffer, int32_t len, const int32_t *timeout_ms=NULL);
        int32_t Close();
		bool IsClosed();
		
        void SetConnTimeout(int32_t ms)
        {
            connect_timeout_ms_ = ms;
        }
        void SetSendTimeout(int32_t ms)
        {
            send_timeout_ms_ = ms;
        }
        void SetRecvTimeout(int32_t ms)
        {
            recv_timeout_ms_ = ms;
        }
        static const int32_t kDefaultConnTimeoutMs = 3000;
        static const int32_t kDefaultSendTimeoutMs = 3000;
        static const int32_t kDefaultRecvTimeoutMs = 3000;
    private:
        std::string ip_;
        uint16_t port_;
        int32_t connect_timeout_ms_;
        int32_t send_timeout_ms_;
        int32_t recv_timeout_ms_;
        int fd_;
};
#endif

