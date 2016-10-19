// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#include <grpc_cb/impl/server/server_writer_impl.h>

#include "server_writer_send_status_cqtag.h"  // for ServerWriterSendStatusCqTag
#include "server_writer_write_cqtag.h"  // for ServerWriterWriteCqTag


namespace grpc_cb {

ServerWriterImpl::ServerWriterImpl(const CallSptr& call_sptr)
    : call_sptr_(call_sptr) {
  assert(call_sptr);
}

ServerWriterImpl::~ServerWriterImpl() {
    Close(Status::OK);
}

bool ServerWriterImpl::Write(
    const ::google::protobuf::Message& response) {
  if (closed_) return false;
  using CqTag = ServerWriterWriteCqTag;
  CqTag* tag = new CqTag(call_sptr_);
  if (tag->Start(response, send_init_md)) {
    send_init_md = false;
    return true;
  }

  delete tag;
  closed_ = true;  // error
  return false;
}

void ServerWriterImpl::Close(const Status& status) {
  if (closed_) return;
  closed_ = true;

  using CqTag = ServerWriterSendStatusCqTag;
  CqTag* tag = new CqTag(call_sptr_);
  // Todo: set init md and trail md
  if (tag->StartSend(status, send_init_md))
    return;
  delete tag;
}

}  // namespace grpc_cb
