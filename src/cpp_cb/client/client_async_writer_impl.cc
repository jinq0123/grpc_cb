// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#include <grpc_cb/impl/client/client_async_writer_impl.h>

#include <cassert>  // for assert()

#include "client_async_writer_impl2.h"  // for ClientAsyncWriterImpl2

namespace grpc_cb {

ClientAsyncWriterImpl::ClientAsyncWriterImpl(const ChannelSptr& channel,
                                             const std::string& method,
                                             const CompletionQueueSptr& cq_sptr)
    : impl2_sptr_(new ClientAsyncWriterImpl2(channel, method, cq_sptr)) {
  assert(cq_sptr);
  assert(channel);
}

ClientAsyncWriterImpl::~ClientAsyncWriterImpl() {
  // XXXX impl2_sptr_->Close();
}

bool ClientAsyncWriterImpl::Write(const MessageSptr& request_sptr) {
  return impl2_sptr_->Write(request_sptr);
}

void ClientAsyncWriterImpl::Close(const CloseHandlerSptr& handler_sptr) {
  impl2_sptr_->Close(handler_sptr);
}

}  // namespace grpc_cb
