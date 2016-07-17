#ifndef APP_QZAP_COMMON_UTILITY_SPP_PROTO_SEND_AND_RECV_UTILITY_H_
#define APP_QZAP_COMMON_UTILITY_SPP_PROTO_SEND_AND_RECV_UTILITY_H_

#include <stdint.h>
#include <string>
#include <vector>
#include "app/qzap/common/base/scoped_ptr.h"
namespace google {
namespace protobuf {
class Message;
}
}
class TCPClientSocket;
class SppProtoSendAndRecv {
 public:
   // Construct with host/port.
   SppProtoSendAndRecv(
     int version,
     const std::string &host,
     short port,
     int timeout_ms);

   // Construct with l5 config.
   SppProtoSendAndRecv(
     int version,
     int l5modid,
     int l5cmd,
     int timeout_ms);
   virtual ~SppProtoSendAndRecv();

   // establish a connection & send & receive
   virtual bool SendAndRecv(
     uint32_t cmd,
     const ::google::protobuf::Message *request,
     ::google::protobuf::Message *response);
   virtual bool RecvFrame(::google::protobuf::Message *response);

   // receive using current connection
   virtual bool Recv(
       ::google::protobuf::Message *response);
 private:
   bool MakesureConnected();
   bool SendAndRecvWhenConnected(
     uint32_t cmd,
     const ::google::protobuf::Message *request,
     ::google::protobuf::Message *response);
   bool RecvWhenConnected(
       ::google::protobuf::Message *response);
   bool RecvFixLengthBuffer(
     char *payload, int payload_length);

   void UpdateL5();
   int version_;
   std::string host_;
   short port_;
   int l5modid_;
   int l5cmd_;
   int timeout_ms_;
   bool init_with_l5_;
   scoped_ptr<TCPClientSocket> socket_;
   uint64_t m_sn;
   std::vector<char> recv_packet_buffer_;
   std::vector<char> payload_;
   DISALLOW_COPY_AND_ASSIGN(SppProtoSendAndRecv);
};
#endif  // APP_QZAP_COMMON_UTILITY_SPP_PROTO_SEND_AND_RECV_UTILITY_H_
