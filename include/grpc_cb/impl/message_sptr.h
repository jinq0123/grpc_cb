// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_IMPL_MESSAGE_SPTR_H
#define GRPC_CB_IMPL_MESSAGE_SPTR_H

#include <memory>

#include <grpc_cb/support/protobuf_fwd.h>  // for Message

namespace grpc_cb {

using MessageSptr = std::shared_ptr<::google::protobuf::Message>;

}  // namespace grpc_cb

#endif  // GRPC_CB_IMPL_CALL_SPTR_H
