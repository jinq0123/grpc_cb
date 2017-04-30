#ifndef GRPC_CB_COMMON_BYTE_BUFFER_H
#define GRPC_CB_COMMON_BYTE_BUFFER_H

#include <string>

struct grpc_byte_buffer;

namespace grpc_cb {
class Status;

namespace ByteBuffer {

void StringToByteBuffer(const std::string& from, grpc_byte_buffer& to);
Status ByteBufferToString(const grpc_byte_buffer& from, std::string& to);

}  // namespace ByteBuffer
}  // namespace grpc_cb
#endif  // GRPC_CB_COMMON_BYTE_BUFFER_H
