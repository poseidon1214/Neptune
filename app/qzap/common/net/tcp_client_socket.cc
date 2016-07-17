#include "app/qzap/common/net/tcp_client_socket.h"
#include <string.h>
#include "poll.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

TCPClientSocket::TCPClientSocket(const std::string &ip, uint16_t port):
    ip_(ip),
    port_(port),
    connect_timeout_ms_(kDefaultConnTimeoutMs),
    send_timeout_ms_(kDefaultSendTimeoutMs),
    recv_timeout_ms_(kDefaultRecvTimeoutMs),
    fd_(-1)
{
}

TCPClientSocket::TCPClientSocket(const std::string &ip, uint16_t port,
        int32_t conn_timeout_ms, int32_t send_timeout_ms, int32_t recv_timeout_ms):
    ip_(ip),
    port_(port),
    connect_timeout_ms_(conn_timeout_ms),
    send_timeout_ms_(send_timeout_ms),
    recv_timeout_ms_(recv_timeout_ms),
    fd_(-1)
{
}

TCPClientSocket::~TCPClientSocket()
{
	Close();
}
int32_t TCPClientSocket::Connect(const int32_t *timeout_ms)
{
    if(fd_ < 0)
    {
        fd_ = socket(AF_INET, SOCK_STREAM, 0);
        if(fd_ < 0)
        {
            return -1;
        }
    }
    int32_t timeout = timeout_ms == NULL?connect_timeout_ms_:*timeout_ms;
    int flags = fcntl(fd_, F_GETFL, 0);
    fcntl(fd_, F_SETFL, flags|O_NONBLOCK);
    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    inet_pton(AF_INET, ip_.c_str(), &(addr.sin_addr));
    addr.sin_port = htons(port_);
    if(::connect(fd_, (sockaddr*)&addr, sizeof(addr)) == -1)
    {
        if(errno == EINPROGRESS || errno == EINTR)
        {
            struct pollfd fds;
            fds.fd = fd_;
            fds.events = POLLIN | POLLOUT;
            int ret = 0;
            do {
              ret = poll(&fds, 1, timeout);
            } while (ret == -1 && errno == EINTR);
            if(ret <= 0)
            {
                Close();
                return -1;
            }
            if((fds.revents&(POLLIN|POLLOUT)) == 0)
            {
                Close();
                return -1;
            }
            int error = 0;
            socklen_t len = sizeof(error);
            if(getsockopt(fd_, SOL_SOCKET, SO_ERROR, &error, &len) < 0 || error != 0)
            {
                Close();
                return -1;
            }
            else
            {
                return 0;
            }

        }
        else
        {
            Close();
            return -1;
        }
    }
    else
    {
        return 0;
    }
}

int32_t TCPClientSocket::Send(const void *buffer, int32_t len, const int32_t *timeout_ms)
{
    if(fd_ < 0)
    {
        return -1;
    }
    int bytes_send = 0;
    int send_ret = 0;
    struct pollfd fds;
    int timeout = timeout_ms == NULL?send_timeout_ms_:*timeout_ms;
    timeval start_time;
    gettimeofday(&start_time, NULL);
    while(timeout>0 && bytes_send < len)
    {
        fds.fd = fd_;
        fds.events = POLLOUT;
        int ret = 0;
        do {
          ret = poll(&fds, 1, timeout);
        } while (ret == -1 && errno == EINTR);
        switch(ret)
        {
            case 0:
                return bytes_send;
            case -1:
                Close();
                return -1;
            default:
                break;
        }
        if((fds.revents&POLLOUT) == 0)
        {
            Close();
            return -1;
        }
        send_ret = ::send(fd_, ((char*)buffer)+bytes_send, len-bytes_send, MSG_NOSIGNAL);
        if(send_ret<=0)
        {
            Close();
            return -1;
        }
        bytes_send += send_ret;
        timeval cur_time;
        gettimeofday(&cur_time, NULL);
        int past_time = ((int64_t)cur_time.tv_sec)*1000+cur_time.tv_usec/1000-((int64_t)start_time.tv_sec*1000+start_time.tv_usec/1000);
        timeout -= past_time;
    }
    return bytes_send;
}


int32_t TCPClientSocket::Recv(void *buffer, int32_t len, const int32_t *timeout_ms)
{
    if(fd_ < 0)
    {
        return -1;
    }
    int bytes_recv = 0;
    int recv_ret = 0;
    int timeout = timeout_ms == NULL?recv_timeout_ms_:*timeout_ms;
    timeval start_time;
    gettimeofday(&start_time, NULL);
    while(timeout>0 && bytes_recv < len)
    {
        recv_ret = RecvOnce(((char*)buffer)+bytes_recv, len-bytes_recv, &timeout);
        if(recv_ret<0)
        {
            return -1;
        }
        bytes_recv += recv_ret;
        timeval cur_time;
        gettimeofday(&cur_time, NULL);
        int past_time = ((int64_t)cur_time.tv_sec)*1000+cur_time.tv_usec/1000-((int64_t)start_time.tv_sec*1000+start_time.tv_usec/1000);
        timeout -= past_time;
    }
    return bytes_recv;
}

int32_t TCPClientSocket::RecvOnce(void *buffer, int32_t len, const int32_t *timeout_ms)
{
    if(fd_ < 0)
    {
        return -1;
    }
    int32_t timeout = timeout_ms == NULL?recv_timeout_ms_:*timeout_ms;
    int recv_ret = 0;
    struct pollfd fds;
    fds.fd = fd_;
    fds.events = POLLIN;
    int ret = 0;
    do {
      ret = poll(&fds, 1, timeout);
    } while (ret < 0 && errno == EINTR);
    switch(ret)
    {
        case 0:
            return 0;
        case -1:
            Close();
            return -1;
    }
    if((fds.revents&POLLIN) == 0)
    {
        Close();
        return -1;
    }
    recv_ret = ::recv(fd_, buffer, len, 0);
    if(recv_ret<=0)
    {
        Close();
        return -1;
    }
    return recv_ret;
}

int TCPClientSocket::Close()
{
    int ret = 0;
    if(fd_ >= 0)
    {
        while((ret=::close(fd_)) == -1 && errno == EINTR);
        if(ret == 0)
        {
            fd_ = -1;
        }
    }
    return ret;
}

bool TCPClientSocket::IsClosed()
{
	if (fd_ == -1)
	{
		return true;
	}
	char ch;
	int recv_ret = recv(fd_, &ch, sizeof(ch), MSG_PEEK);
	if (recv_ret == 1)
	{
		return false;
	}
	else if (recv_ret == 0)
	{
		//server shutdown connection.
		Close();
		return true;
	}
	else if(errno == EAGAIN)
	{
		// no data
		return false;
	}
	else
	{
		//error
		Close();
		return true;
	}
}
