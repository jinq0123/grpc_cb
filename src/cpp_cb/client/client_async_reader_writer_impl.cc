// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#include <grpc_cb/impl/client/client_async_reader_writer_impl.h>

#include <cassert>

#include "client_async_reader_writer_impl2.h"  // for ClientAsyncReaderWriterImpl2

namespace grpc_cb {

using Sptr = std::shared_ptr<ClientAsyncReaderWriterImpl>;

// Todo: BlockingGetInitMd();

ClientAsyncReaderWriterImpl::ClientAsyncReaderWriterImpl(
    const ChannelSptr& channel, const std::string& method,
    const CompletionQueueSptr& cq_sptr, const StatusCallback& on_status)
    : impl2_sptr_(new ClientAsyncReaderWriterImpl2(channel, method, cq_sptr,
                                                   on_status)) {
  assert(cq_sptr);
}

ClientAsyncReaderWriterImpl::~ClientAsyncReaderWriterImpl() {
  impl2_sptr_->CloseWriting();  // impl2_sptr_ will live on.
}

bool ClientAsyncReaderWriterImpl::Write(const MessageSptr& msg_sptr) {
  return impl2_sptr_->Write(msg_sptr);
}

void ClientAsyncReaderWriterImpl::CloseWriting() {
  impl2_sptr_->CloseWriting();
}

void ClientAsyncReaderWriterImpl::ReadEach(
    const ClientAsyncReadHandlerSptr& handler_sptr) {
  impl2_sptr_->ReadEach(handler_sptr);
}

}  // namespace grpc_cb
