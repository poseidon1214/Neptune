#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "thirdparty/spp/sppincl.h"
#include "base_class_old/include/qzone_protocol.h"
#include "thirdparty/gflags/gflags.h"
#include "thirdparty/glog/logging.h"
#include "app/qzap/common/utility/spp_proto_send_and_recv_test.pb.h"
using namespace tbase::tcommu;
using namespace spp::comm;
namespace {
static const uint8_t kSppProtoSendAndRecvTestVersion = 0xef;
static const uint8_t kSppProtoSendAndRecvTestCommand = 1;
int serial_no = 1;
}

extern "C" int spp_handle_init(void* , void* arg2) {
  CServerBase* pServerBase = (CServerBase*)arg2;
  if (pServerBase == NULL) {
    printf("unhandled error, the serverbase pointer is NULL.");
    return -1;
  }

  switch(pServerBase->servertype()) {
    case SERVER_TYPE_PROXY:
      google::InitGoogleLogging("SppProtoSendAndRecvProxy");
      FLAGS_log_dir = "/tmp";
      LOG(INFO) << "the proxy module is initialized successfully.";
      break;
    case SERVER_TYPE_WORKER:
      google::InitGoogleLogging("SppProtoSendAndRecvWorker");
      FLAGS_log_dir = "/tmp";
      LOG(INFO) << "The worker module is initialized successfully.";
      break;
    default:
      LOG(INFO) << "this won't occur, something must going wrong!";
      return -1;
  }
  return 0;
}

static inline int CheckQZoneProto(const char *data, int length) {
  if (data == NULL) {
    LOG(ERROR) << "CheckQZoneProto error, data is null";
    return 0;
  }
  if (length <= 0) {
    LOG(ERROR) << "CheckQZoneProto error, length: " << length;
    return 0;
  }

  if (QzoneProtocolSOH != data[0]) {
    LOG(ERROR) << "CheckQZoneProto error, not a QzoneProtocol";
    return -1;
  }

  static const int kHeadLength = sizeof(char) + sizeof(QzoneProtocolHead);
  if (length < kHeadLength) {
    return 0;
  }

  QzoneProtocolPtr packet = (QzoneProtocolPtr)(data);
  int packet_length = ntohl(packet->head.len);
  if (packet_length < kHeadLength) {
    LOG(ERROR) << "CheckQZoneProto error, packet length: " << packet_length << " is invalid";
    return -2;
  }

  if (length < packet_length) {
    return 0;
  }
  return packet_length;
}

extern "C" int spp_handle_input(unsigned, void* arg1, void*) {
  LOG(INFO) << "spp_handle_input";
  blob_type* blob = (blob_type*)arg1;
  int ret =  CheckQZoneProto(blob->data, blob->len);
  if (ret <= 0) {
    LOG(WARNING) << "receive a illegal package, with length " << blob->len;
  }
  return ret;
}

static inline void SendErrorResponse(
  CTCommu *commu,
  int flow) {
  QzoneProtocolPacket packet;
  char *buffer = NULL;
  int buffer_length = 0;
  int ret = packet.MapPacketBuffer(buffer, buffer_length);
  if (ret != 0) {
    LOG(ERROR) << "SendErrorResponse MapPacketBuffer fail";
    return;
  }
  packet.SetHead(kSppProtoSendAndRecvTestVersion, kSppProtoSendAndRecvTestCommand); 
  packet.SetSerialNo(serial_no++);
  packet.SetServerResponse(QzoneServerFailed, 0);
  packet.AppendData("\0", 1);
  packet.Output(buffer, buffer_length);
  blob_type response_blob;
  response_blob.data = buffer;
  response_blob.len = buffer_length;
  ret = commu->sendto(flow, &response_blob, NULL);
  if (ret != 0) {
    LOG(ERROR) << "SendResponse to : " << flow << " failed";
    return;
  }
}

extern "C" int spp_handle_process(unsigned flow, void* arg1, void*) {
  LOG(INFO) << "spp_handle_process, flow;" << flow;
  blob_type *blob = (blob_type*)arg1;
  CTCommu *commu  = (CTCommu*)blob->owner;
  QzoneProtocolPacket recv_packet;
  int ret = recv_packet.MapPacketBuffer(blob->data, blob->len);
  if (0 != ret) {
    LOG(ERROR) << "MapPacketBuffer error. iRet=" << ret;
    SendErrorResponse(commu, flow);
    return ret;
  }
  LOG(INFO) << "MapPacketBuffer blob len: " << blob->len;

  if (recv_packet.mappedVersion() != kSppProtoSendAndRecvTestVersion) {
    LOG(ERROR) << "receive a mismatched version."
      << recv_packet.mappedVersion();
    SendErrorResponse(commu, flow);
    return -1;
  }
  SppProtoSendAndRecvTestRequest request;
  
  LOG(INFO) << "recv_packet mappedLen: " << recv_packet.mappedLen();
  if (!request.ParseFromArray(recv_packet.body(), recv_packet.bodyMaxLen())) {
    LOG(ERROR) << "Fail to ParseFromArray recv packet, recv_packet length:" << recv_packet.bodyMaxLen();
    SendErrorResponse(commu, flow);
    return -1;
  }
  SppProtoSendAndRecvTestResponse response;
  response.set_c(request.a());
  response.set_d(request.b());
  response.set_blob(request.blob());
  std::string response_buffer;
  if (!response.AppendToString(&response_buffer)) {
    LOG(ERROR) << "response Fail to AppendToString";
    SendErrorResponse(commu, flow);
    return -1;
  }
  
  QzoneProtocolPacket packet;
  const int response_packet_length = response_buffer.size() + sizeof(packet);
  ret = packet.CreatePacket(response_packet_length);
  if (ret != 0) {
    LOG(ERROR) << "Create fail, length: " << response_packet_length;
    SendErrorResponse(commu, flow);
    return -1;
  }
  packet.SetHead(kSppProtoSendAndRecvTestVersion, kSppProtoSendAndRecvTestCommand); 
  packet.SetSerialNo(serial_no++);
  packet.SetServerResponse(QzoneServerSucc, 0);
  packet.AppendData(response_buffer.c_str(), response_buffer.length());
  char *buffer = NULL;
  int buffer_length;
  packet.Output(buffer, buffer_length);
  blob_type response_blob;
  response_blob.data = buffer;
  response_blob.len = buffer_length;
  ret = commu->sendto(flow, &response_blob, NULL);
  if (ret != 0) {
    LOG(ERROR) << "SendResponse to : " << flow << " failed";
  }
  return  ret;
}

extern "C" void spp_handle_fini(void* , void* arg2) {
  CServerBase* pServerBase = (CServerBase*)arg2;

  if (pServerBase == NULL) {
    LOG(INFO) << "unhandled error, the serverbase pointer is NULL.";
    return;
  }

  switch (pServerBase->servertype()) {
    case SERVER_TYPE_PROXY:
      LOG(ERROR) << "the proxy module is initialized successfully.";
      break;
    case SERVER_TYPE_WORKER:
      LOG(ERROR) << "The worker module is finalized successfully.";
      break;
    default:
      LOG(ERROR) << "this won't occur, something must going wrong!";
      return;
  }
  return;
}
