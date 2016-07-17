// Copyright (C), 1998-2015, Tencent
// Author: Li, Jiliang neoli@tencent.com
// Date: 2015年11月25日

#include "common/encoding/protobuf_utility.h"

#include "google/protobuf/message.h"
#include "google/protobuf/io/gzip_stream.h"
#include "google/protobuf/io/zero_copy_stream_impl_lite.h"
#include "thirdparty/glog/logging.h"
#include "thirdparty/zlib/zlib.h"

using google::protobuf::io::GzipInputStream;
using google::protobuf::io::GzipOutputStream;
using google::protobuf::io::StringOutputStream;
using google::protobuf::io::ArrayInputStream;

namespace gdt {
bool SerializeToGzippedString(const google::protobuf::Message& message,
    std::string *output) {
  google::protobuf::io::GzipOutputStream::Options compress_options;
  compress_options.format = google::protobuf::io::GzipOutputStream::ZLIB;
  compress_options.compression_level = Z_BEST_SPEED;
  StringOutputStream string_stream(output);
  GzipOutputStream gzip_stream(&string_stream, compress_options);
  if (!message.SerializeToZeroCopyStream(&gzip_stream) ||
      !gzip_stream.Close()) {
    LOG(ERROR) << "Serialize to Gzip failed, error:" <<
        gzip_stream.ZlibErrorMessage();
    return false;
  }
  return true;
}

bool ParseFromGzippedString(const std::string& input,
    google::protobuf::Message* message) {
  ArrayInputStream array(input.data(), input.size());
  GzipInputStream gzip_input(&array);
  bool ret = message->ParseFromZeroCopyStream(&gzip_input);
  bool zlib_ok = gzip_input.ZlibErrorCode() == Z_OK ||
      gzip_input.ZlibErrorCode() == Z_STREAM_END;
  if (!zlib_ok) {
    LOG(ERROR) << "Parse from Gzip failed, error:" <<
        gzip_input.ZlibErrorMessage();
  }
  return ret && zlib_ok;
}

google::protobuf::Message* CreateProtoMessageByName(const std::string& name) {
  google::protobuf::Message * message = NULL;
  const google::protobuf::Descriptor* descriptor =
      google::protobuf::DescriptorPool::generated_pool()
      ->FindMessageTypeByName(name);
  if (descriptor != NULL) {
    const google::protobuf::Message* prototype =
        google::protobuf::MessageFactory::generated_factory()
        ->GetPrototype(descriptor);
    if (prototype) {
      message = prototype->New();
    }
  }
  return message;
}

}  // namespace gdt
