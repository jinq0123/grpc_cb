// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CLIENT_ASYNC_WRITER_IMPL2_SPTR_H
#define GRPC_CB_CLIENT_ASYNC_WRITER_IMPL2_SPTR_H

#include <memory>

namespace grpc_cb {

class ClientAsyncWriterImpl2;
using ClientAsyncWriterImpl2Sptr = std::shared_ptr<ClientAsyncWriterImpl2>;

}  // namespace grpc_cb
#endif  // GRPC_CB_CLIENT_ASYNC_WRITER_IMPL2_SPTR_H
