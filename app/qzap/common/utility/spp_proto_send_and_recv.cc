#include "app/qzap/common/base/walltime.h"
#include "app/qzap/common/net/tcp_client_socket.h"
#include "base_class_old/include/qzone_protocol.h"
#include "app/qzap/common/utility/spp_proto_send_and_recv.h"
#include "base_class_old/src/csocket/csocket.h"
#include "thirdparty/protobuf/message.h"
#include "thirdparty/qos_client/qos_client.h"
#include "thirdparty/glog/logging.h"
SppProtoSendAndRecv::SppProtoSendAndRecv(
  int version,
  const std::string &host,
  short port,
  int timeout_ms) :
  version_(version),
  host_(host),
  port_(port),
  l5modid_(-1),
  l5cmd_(-1),
  timeout_ms_(timeout_ms),
  init_with_l5_(false) {
    m_sn = 0;
}

SppProtoSendAndRecv::SppProtoSendAndRecv(
  int version,
  int l5modid,
  int l5cmd,
  int timeout_ms) :
  version_(version),
  l5modid_(l5modid),
  l5cmd_(l5cmd),
  timeout_ms_(timeout_ms),
  init_with_l5_(true) {
    m_sn = 0;
}

SppProtoSendAndRecv::~SppProtoSendAndRecv() {
}

bool SppProtoSendAndRecv::Recv(
    ::google::protobuf::Message *response) {

        if (socket_.get() == NULL || socket_->IsClosed()) { // we must use the previous socket
            LOG(ERROR) << "SppProtoSendAndRecv::NoSendAndRecv socket_ already closed";
            return false;
        }

        return RecvFrame(response);
}

bool SppProtoSendAndRecv::SendAndRecv(
  uint32_t cmd,
  const ::google::protobuf::Message *request,
  ::google::protobuf::Message *response) {
  if (request == NULL) {
    LOG(ERROR) << "SppProtoSendAndRecv::SendAndRecv request is NULL";
    return false;
  }

  if (!MakesureConnected()) {
    LOG(WARNING) << "SppProtoSendAndRecv::SendAndRecv fail to connected";
    return false;
  }

  double begin_time = WallTime_Now();
  bool ret = SendAndRecvWhenConnected(cmd, request, response);
  if (init_with_l5_) {
     //L5 update
    int32_t sig_time= static_cast<int32_t>((WallTime_Now() - begin_time)) * 1000000;
    QOSREQUEST l5_req;
    l5_req._host_ip = host_;
    l5_req._host_port = port_;
    l5_req._modid = l5modid_;
    l5_req._cmd = l5cmd_;
    std::string error_msg;
    int32_t l5_ret = ApiRouteResultUpdate(
      l5_req, ret ? 0 : -1, sig_time, error_msg);

    if (l5_ret) {
      LOG(ERROR) << "SppProtoSendAndRecv::SendAndRecv l5_ret: " << l5_ret;
      return false;
    }
  }
  return ret;
}

bool SppProtoSendAndRecv::MakesureConnected() {
  if (socket_.get() != NULL && !socket_->IsClosed()) {
    return true;
  }
  if (init_with_l5_) {
    QOSREQUEST req;
    req._modid = l5modid_;
    req._cmd = l5cmd_;
    string err_msg;
    float tm_out = 0.2;

    int ret = ApiGetRoute(req, tm_out, err_msg);
    if (ret < 0) {
      if (ret == QOS_RTN_OVERLOAD) {
        LOG(ERROR) << "Service overload, modid: " << req._modid
          << " cmd: " << req._cmd;
        return false;
      }
      LOG(ERROR) << "ApiGetRoute failed, ret: " << ret
        << " err: " << err_msg;
      return false;
    }
    host_ = req._host_ip;
    port_ = req._host_port;
  }
  socket_.reset(new TCPClientSocket(host_, port_, timeout_ms_, timeout_ms_, timeout_ms_));
  int ret = socket_->Connect();
  if (0 != ret) {
    LOG(ERROR) << "MakesureConnected fail, ret: " << ret
      << " host_: " << host_ << " port: " << port_ << " timeout_ms:" << timeout_ms_ << ": ret: " << ret
      << " error: " << errno << " : " << strerror(errno);
    return false;
  }
  return true;
}

bool SppProtoSendAndRecv::SendAndRecvWhenConnected(
  uint32_t cmd,
  const ::google::protobuf::Message *request,
  ::google::protobuf::Message *response) {
  int ret;
  std::string request_buffer;
  if (!request->AppendToString(&request_buffer)) {
    LOG(ERROR) << "Fail to serialize request: " << request->DebugString();
    return false;
  }

  QzoneProtocolPacket packet;
  const int packet_length = request_buffer.size() + sizeof(packet);
  VLOG(2) << "packet_length: " << packet_length << " request_buffer size: " << request_buffer.size();
  ret = packet.CreatePacket(packet_length);
  if (ret != 0) {
    LOG(ERROR) << "Fail to CreatePacket, packet_length:" << packet_length;
    return false;
  }

  packet.SetHead(
   static_cast<uint8_t>(version_),
   static_cast<uint32_t>(cmd));
  packet.SetSerialNo(m_sn++);
  packet.SetServerResponse(QzoneServerSucc, 0);
  packet.AppendData(request_buffer.c_str(), request_buffer.size());
  char *packet_buffer;
  int packet_buffer_length;
  ret = packet.Output(packet_buffer, packet_buffer_length);
  if (ret != 0) {
    LOG(ERROR) << "SppProtoSendAndRecv::SendAndRecv packet output fail, ret: " << ret;
    return false;
  }

  VLOG(2) << "Write to socket, length: " << packet_buffer_length;
  ret = socket_->Send(packet_buffer, packet_buffer_length );
  if (ret != packet_buffer_length) {
    LOG(ERROR) << "SppProtoSendAndRecv::SendAndRecv ret: " << ret
      << " ip: " << host_ << " port: " << port_
      << " errno: " << errno << " err: " << strerror(errno);
    socket_.reset();
    return false;
  }
  VLOG(2) << "Write to socket done: " << packet_buffer_length;
  VLOG(2) << " ip: " << host_ << " port: " << port_;
  return RecvFrame(response);
}

bool SppProtoSendAndRecv::RecvFrame(::google::protobuf::Message *response) {
    QzoneProtocolPacket recv_packet;
    int recv_packet_length = recv_packet.headSize();
    recv_packet_buffer_.resize(recv_packet_length);
    if (!RecvFixLengthBuffer(
        &recv_packet_buffer_[0], recv_packet_length)) {
            LOG(WARNING) << "SppProtoSendAndRecv::RecvWhenConnected RecvFixBufferLength fail";
            return false;
    }

    int ret = recv_packet.MapPacketHeadBuffer(&recv_packet_buffer_[0], recv_packet_length);

    if (0 != ret) {
        LOG(ERROR) << "SppProtoSendAndRecv::RecvWhenConnected MapPacketHeadBuffer fail, ret: " << ret;
        return false;
    }

    if (recv_packet.mappedSerialNo() != m_sn -1)    // we use the previous m_sn as we are the same session
    {
        LOG(WARNING) << "recv serialno = " << recv_packet.mappedSerialNo() << ", send sn = "<< m_sn - 1 << ", respFlag = " << (int)recv_packet.m_pPacket->head.serverResponseFlag;
    }

    if (0 != recv_packet.CheckPacketBuffer(&recv_packet_buffer_[0])) {
        LOG(WARNING) << "SppProtoSendAndRecv::RecvWhenConnected CheckPacketBuffer fail";
        return false;
    }


    if (0 != recv_packet.mappedServerResponseFlag()) {
        LOG(ERROR) << "SppProtoSendAndRecv::RecvWhenConnected recv_packet.mappedServerResponseFlag fail, ret: "
            << recv_packet.mappedServerResponseFlag();
        return false;
    }

    if (0 != recv_packet.mappedServerResponseInfo()) {
        LOG(ERROR) << "SppProtoSendAndRecv::RecvWhenConnected recv_packet.mappedServerResponseInfo fail, ret: "
            << recv_packet.mappedServerResponseInfo();
        return false;
    }

    int32_t payload_length = recv_packet.mappedLen() - recv_packet.headSize();
    payload_.resize(payload_length);

    if (!RecvFixLengthBuffer(&payload_[0], payload_length)) {
        LOG(WARNING) << "SppProtoSendAndRecv::RecvWhenConnected receive payload : "
            << payload_length << " fail";
        return false;
    }

    if (response == NULL) {
        return true;
    }

    const int response_size = recv_packet.bodySize(recv_packet.mappedLen());
    if (!response->ParseFromArray(&payload_[0], response_size)) {
        LOG(ERROR) << "SppProtoSendAndRecv::RecvWhenConnected response ParseFromArray fail, response_size = " << response_size;

        return false;
    }
    return true;
}



bool SppProtoSendAndRecv::RecvFixLengthBuffer(
  char *payload, int payload_length) {
  if (socket_->Recv(payload, payload_length) != payload_length) {
      LOG(WARNING) << "SppProtoSendAndRecv::RecvFixLengthBuffer fail "
        << " recv_length: " << payload_length
        << " errno: " << errno
        << " error_msg: " << strerror(errno)
        << " timeout: " << timeout_ms_;
      socket_.reset();
      return false;
  }
  VLOG(2) << "RecvWithTimeout success, recv_length: " << payload_length;
  return true;
}
