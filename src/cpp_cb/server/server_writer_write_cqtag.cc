// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#include "server_writer_write_cqtag.h"

#include <grpc_cb/impl/call.h>                // for StartBatch()
#include <grpc_cb/impl/call_operations.h>     // for CallOperations

namespace grpc_cb {

bool ServerWriterWriteCqTag::Start(
    const ::google::protobuf::Message& message, bool send_init_md) {
  CallOperations ops;
  if (send_init_md) {
    // Todo: set init_md
    ops.SendInitMd(cod_send_init_md_);
  }
  ops.SendMsg(message, cod_send_msg_);
  return GetCallSptr()->StartBatch(ops, this);
}

}  // namespace grpc_cb
