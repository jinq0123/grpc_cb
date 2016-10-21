// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#include <grpc_cb/impl/server/server_writer_impl.h>

#include <thread>
#include <google/protobuf/message.h>

#include "server_writer_send_status_cqtag.h"  // for ServerWriterSendStatusCqTag
#include "server_writer_write_cqtag.h"  // for ServerWriterWriteCqTag

namespace grpc_cb {

ServerWriterImpl::ServerWriterImpl(const CallSptr& call_sptr)
    : call_sptr_(call_sptr) {
  assert(call_sptr);
}

ServerWriterImpl::~ServerWriterImpl() {
  // Because ServerWriterWriteCqTag will share this.
  assert(queue_.empty());
  BlockingClose(Status::OK);
}

//bool ServerWriterImpl::Write(
//    const ::google::protobuf::Message& response) {
//  Guard g(mtx_);
//
//  if (closed_) return false;
//  using CqTag = ServerWriterWriteCqTag;
//  CqTag* tag = new CqTag(call_sptr_);
//  if (tag->Start(response, send_init_md)) {
//    send_init_md = false;
//    return true;
//  }
//
//  delete tag;
//  closed_ = true;  // error
//  return false;
//}

bool ServerWriterImpl::Write(
    const ::google::protobuf::Message& response) {
  bool is_high = false;
  {
    Guard g(mtx_);
    if (queue_.size() >= high_queue_size_)
      is_high = true;
  }
  if (is_high)
    BlockingWrite(response);
  else
    AsyncWrite(response);

  return false;  // XXX
}

bool ServerWriterImpl::BlockingWrite(
    const ::google::protobuf::Message& response) {
  AsyncWrite(response);  // Will trigger writing...

  while (GetQueueSize())
    std::this_thread::yield();

  return false;  // XXX check error?
}

void ServerWriterImpl::AsyncWrite(
    const ::google::protobuf::Message& response) {
  Guard g(mtx_);

  MessageSptr p(response.New());
  p->CopyFrom(response);
  queue_.push(p);

  // XXX Trigger writing...
}

//void ServerWriterImpl::Close(const Status& status) {
//  Guard g(mtx_);
//
//  if (closed_) return;
//  closed_ = true;
//
//  using CqTag = ServerWriterSendStatusCqTag;
//  CqTag* tag = new CqTag(call_sptr_);
//  // Todo: set init md and trail md
//  if (tag->StartSend(status, send_init_md))
//    return;
//  delete tag;
//}

void ServerWriterImpl::BlockingClose(const Status& status) {
  AsyncClose(status);
  while (GetQueueSize())
    std::this_thread::yield();
}

void ServerWriterImpl::AsyncClose(const Status& status) {
  Guard g(mtx_);

  if (closed_ || close_status_sptr_)
    return;

  close_status_sptr_.reset(new Status(status));
  // XXX Trigger sending.
}

void ServerWriterImpl::TryToWriteNext() {
  Guard g(mtx_);

  if (closed_) return;
  if (!queue_.empty()) {
    WriteNextMessage();
    return;
  }
  if (close_status_sptr_) {
    SendStatus();
    closed_ = true;
  }
}

void ServerWriterImpl::SendStatus() const {
  assert(!closed_);
  assert(close_status_sptr_);

  const Status& status = *close_status_sptr_;
  using CqTag = ServerWriterSendStatusCqTag;
  CqTag* tag = new CqTag(call_sptr_);
  // Todo: set init md and trail md
  if (tag->StartSend(status, send_init_md))
    return;
  delete tag;
}

void ServerWriterImpl::WriteNextMessage() {
}

}  // namespace grpc_cb
