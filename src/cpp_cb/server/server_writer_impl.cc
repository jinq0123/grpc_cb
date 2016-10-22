// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#include <grpc_cb/impl/server/server_writer_impl.h>

#include <thread>
#include <google/protobuf/message.h>

#include "server_writer_send_status_cqtag.h"  // for ServerWriterSendStatusCqTag
#include "server_writer_write_cqtag.h"  // for ServerWriterWriteCqTag

namespace grpc_cb {

ServerWriterImpl::ServerWriterImpl(const CallSptr& call_sptr,
                                   const CompletionQueueSptr& cq_sptr)
    : call_sptr_(call_sptr), cq_sptr_(cq_sptr) {
  assert(call_sptr);
  assert(cq_sptr);
}

ServerWriterImpl::~ServerWriterImpl() {
  // Because ServerWriterWriteCqTag has a ServerWriterImpl sptr.
  assert(queue_.empty());
  BlockingClose(Status::OK);
}

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
  AsyncWrite(response);  // Will trigger sending.

  while (GetQueueSize())
    std::this_thread::yield();

  return false;  // XXX check error?
}

void ServerWriterImpl::AsyncWrite(
    const ::google::protobuf::Message& response) {
  Guard g(mtx_);

  if (closed_ || close_status_sptr_)
    return;

  if (is_sending_) {
    MessageSptr p(response.New());
    p->CopyFrom(response);
    queue_.push(p);
    return;
  }

  assert(queue_.empty());
  SendMsg(response);
}

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
  if (is_sending_) return;
  assert(queue_.empty());
  SendStatus();
}

void ServerWriterImpl::TryToWriteNext() {
  Guard g(mtx_);

  if (closed_) return;

  is_sending_ = false;  // Prev msg sending completed.
  if (!queue_.empty()) {
    MessageSptr msg = queue_.front();
    queue_.pop();
    assert(msg);
    SendMsg(*msg);
    return;
  }

  if (close_status_sptr_) {
    SendStatus();
    return;
  }

  // No message and no close.
  is_sending_ = false;
}

void ServerWriterImpl::SendStatus() {
  assert(!closed_);
  assert(close_status_sptr_);
  assert(queue_.empty());

  closed_ = true;
  const Status& status = *close_status_sptr_;
  using CqTag = ServerWriterSendStatusCqTag;
  CqTag* tag = new CqTag(call_sptr_);
  // Todo: set init md and trail md
  if (tag->StartSend(status, send_init_md_))
    return;
  delete tag;
}

void ServerWriterImpl::SendMsg(const ::google::protobuf::Message& msg) {
  assert(!closed_);
  assert(!is_sending_);
  is_sending_ = true;

  // CqTag shares this and will TryToWriteNext() on cq completion.
  using CqTag = ServerWriterWriteCqTag;
  CqTag* tag = new CqTag(call_sptr_, shared_from_this());
  if (tag->Start(msg, send_init_md_)) {
    send_init_md_ = false;
    return;
  }

  delete tag;
  closed_ = true;  // error
  // Todo: do sth. on error?
}

}  // namespace grpc_cb
