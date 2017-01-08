#ifndef GRPC_CB_IMPL_PROTO_UTILS_H
#define GRPC_CB_IMPL_PROTO_UTILS_H

#include <grpc_cb/support/protobuf_fwd.h>  // for Message

struct grpc_byte_buffer;

namespace grpc_cb {

class Status;

namespace Proto {

// Serialize the msg into a buffer created inside the function. The caller
// should destroy the returned buffer when done with it. If serialization fails,
// false is returned and buffer is left unchanged.
Status Serialize(const ::google::protobuf::Message& msg,
                 grpc_byte_buffer** buffer);

// The caller keeps ownership of buffer and msg.
// max_msg_size <= 0 means no limit.
Status Deserialize(grpc_byte_buffer* buffer, ::google::protobuf::Message* msg,
                   int max_msg_size);

}  // namespace Proto
}  // namespace grpc_cb

#endif  // GRPC_CB_IMPL_PROTO_UTILS_H
