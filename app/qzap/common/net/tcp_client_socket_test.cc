#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <string>
#include "app/qzap/common/base/thread.h"
#include "app/qzap/common/utility/time_utility.h"
#include "tcp_client_socket.h"
#include "thirdparty/gtest/gtest.h"

class TCPClientSocketTest : public testing::Test
{
};

std::string listen_ip("127.0.0.1");
volatile uint16_t listen_port = 9000;
const int32_t pkg_size = 64;
volatile bool server_start = false;
class TestServer
{
    public:
        TestServer():server_socket_(-1), connection_socket_(-1)
        {
        }
        virtual ~TestServer()
        {
            if (connection_socket_ != -1)
            {
                close(connection_socket_);
            }
            if (server_socket_ != -1)
            {
                close(server_socket_);
            }
            server_start = false;
        }
        bool Listen()
        {
            server_socket_ = socket(AF_INET, SOCK_STREAM, 0);
            sockaddr_in listen_addr;
            memset(&listen_addr, 0, sizeof(listen_addr));
            listen_addr.sin_family = AF_INET;
            while (true)
            {
                listen_addr.sin_port = htons(listen_port);
                inet_pton(AF_INET, listen_ip.c_str(),&(listen_addr.sin_addr));
                if (bind(server_socket_, (sockaddr*)&listen_addr, sizeof(listen_addr)) == 0 && listen(server_socket_, 1024) == 0)
                {
                    return true;
                }
                if (errno == EADDRINUSE)
                {
                    ++listen_port;
                }
            }
        }
        int Accept()
        {
            if (server_socket_ == -1)
            {
                return -1;
            }
            sockaddr client_addr;
            socklen_t addr_len = sizeof(client_addr);
            server_start = true;
            connection_socket_ = accept(server_socket_, &client_addr, &addr_len);
            return connection_socket_;
        }
        bool Recv(std::string &buffer)
        {
            char local_buffer[pkg_size];
            if (recv(connection_socket_, local_buffer, pkg_size, 0) == pkg_size)
            {
                buffer.assign(local_buffer, pkg_size);
                return true;
            }
            else
            {
                return false;
            }
        }
        bool Send(const std::string &buffer)
        {
            return send(connection_socket_, buffer.c_str(), buffer.size(), 0);
        }
    private:
        int server_socket_;
        int connection_socket_;
};

void RecvSendOnce()
{
    TestServer server;
    ASSERT_TRUE(server.Listen()); 
    ASSERT_TRUE(server.Accept() != -1);
    std::string recv_buffer;
    ASSERT_TRUE(server.Recv(recv_buffer));
    SleepMilliseconds(2000);
    ASSERT_TRUE(server.Send(recv_buffer));
}


TEST_F(TCPClientSocketTest, Test1)
{
    Thread thread(NewCallback(RecvSendOnce));
    thread.Start();
    while(!server_start)
    {
        SleepMilliseconds(5);
    }
    TCPClientSocket client_sock(listen_ip, listen_port);
    ASSERT_EQ(client_sock.Connect(), 0);
    std::string send_buf(pkg_size, 'a');
    std::string recv_buf(pkg_size, 'b');
    ASSERT_EQ(client_sock.Send(send_buf.c_str(), send_buf.size()), pkg_size);
    ASSERT_FALSE(client_sock.IsClosed());
    ASSERT_EQ(client_sock.Recv(const_cast<char*>(recv_buf.c_str()), pkg_size), pkg_size);
    ASSERT_EQ(send_buf, recv_buf);
    while (server_start)
    {
        SleepMilliseconds(5);
    }
    SleepMilliseconds(10);
    ASSERT_TRUE(client_sock.IsClosed());
}

TEST_F(TCPClientSocketTest, Test2)
{
    Thread thread(NewCallback(RecvSendOnce));
    thread.Start();
    while(!server_start)
    {
        SleepMilliseconds(5);
    }
    TCPClientSocket client_sock(listen_ip, listen_port, 3000, 3000, 3000);
    ASSERT_EQ(client_sock.Connect(), 0);
    std::string send_buf(pkg_size, 'a');
    std::string recv_buf(pkg_size, 'b');
    ASSERT_EQ(client_sock.Send(send_buf.c_str(), send_buf.size()), pkg_size);
    ASSERT_FALSE(client_sock.IsClosed());
    ASSERT_EQ(client_sock.Recv(const_cast<char*>(recv_buf.c_str()), pkg_size), pkg_size);
    ASSERT_EQ(send_buf, recv_buf);
    while (server_start)
    {
        SleepMilliseconds(5);
    }
    SleepMilliseconds(10);
    ASSERT_TRUE(client_sock.IsClosed());
}
