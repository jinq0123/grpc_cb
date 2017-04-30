#include "byte_buffer.h"

#include <grpc_cb/status.h>  // for Status

namespace grpc_cb {
namespace ByteBuffer {

void StringToByteBuffer(const std::string& from, grpc_byte_buffer& to) {
  // XXX
}

Status ByteBufferToString(const grpc_byte_buffer& from, std::string& to) {
  // XXX
  return Status::UNIMPLEMENTED;
}

}  // namespace ByteBuffer
}  // namespace grpc_cb

