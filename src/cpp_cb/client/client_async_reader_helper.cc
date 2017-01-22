// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#include "client_async_reader_helper.h"

#include <cassert>  // for assert()

#include <grpc_cb/impl/client/client_async_read_handler.h>  // for HandleMsg()

#include "client_reader_async_read_cqtag.h"  // for ClientReaderAsyncReadCqTag

namespace grpc_cb {

ClientAsyncReaderHelper::ClientAsyncReaderHelper(CallSptr call_sptr,
    const ClientAsyncReadHandlerSptr& read_handler_sptr, const OnEnd& on_end)
    : call_sptr_(call_sptr),
      read_handler_sptr_(read_handler_sptr),
      on_end_(on_end) {
  assert(call_sptr);
  assert(read_handler_sptr);
  assert(on_end);
}

ClientAsyncReaderHelper::~ClientAsyncReaderHelper() {}

// Setup to read each.
void ClientAsyncReaderHelper::Start() {
  if (started_) return;
  started_ = true;
  Next();
}

// Setup next async read.
void ClientAsyncReaderHelper::Next() {
  assert(started_);
  if (aborted_)  // Maybe writer failed.
    return;

  auto* tag = new ClientReaderAsyncReadCqTag(call_sptr_);
  auto sptr = shared_from_this();
  tag->SetOnRead([sptr, tag]() { sptr->OnRead(*tag); });
  if (tag->Start()) return;

  delete tag;
  status_.SetInternalError("Failed to async read server stream.");
  on_end_();
}

void ClientAsyncReaderHelper::OnRead(ClientReaderAsyncReadCqTag& tag) {
  if (aborted_)  // Maybe writer failed.
    return;
  assert(status_.ok());
  assert(on_end_);
  if (!tag.HasGotMsg()) {
    // End of read. Do not recv status in Reader. Do it after all reading and writing.
    on_end_();
    return;
  }

  status_ = tag.GetResultMsg(read_handler_sptr_->GetMsg());
  if (!status_.ok()) {
    on_end_();
    return;
  }

  read_handler_sptr_->HandleMsg();
  Next();
}

}  // namespace grpc_cb
