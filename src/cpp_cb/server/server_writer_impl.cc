// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#include <grpc_cb/impl/server/server_writer_impl.h>

#include <thread>  // for yield()

#include <google/protobuf/message.h>

#include <grpc_cb/impl/message_sptr.h>  // for MessageSptr

#include "server_writer_send_status_cqtag.h"  // for ServerWriterSendStatusCqTag
#include "server_writer_write_cqtag.h"        // for ServerWriterWriteCqTag

namespace grpc_cb {

ServerWriterImpl::ServerWriterImpl(const CallSptr& call_sptr)
    : call_sptr_(call_sptr) {
  assert(call_sptr);
}

ServerWriterImpl::~ServerWriterImpl() {
  // Because ServerWriterWriteCqTag has a ServerWriterImpl sptr.
  assert(queue_.empty());
  SyncClose(Status::OK);
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
    return SyncWrite(response);
  else
    return AsyncWrite(response);
}

bool ServerWriterImpl::SyncWrite(
    const ::google::protobuf::Message& response) {
  if (!AsyncWrite(response))  // Will trigger sending.
    return false;

  while (GetQueueSize())  // need to clear queue on error
    std::this_thread::yield();

  Guard g(mtx_);
  return closed_;
}

bool ServerWriterImpl::AsyncWrite(
    const ::google::protobuf::Message& response) {
  Guard g(mtx_);

  if (closed_ || close_status_uptr_)
    return false;

  if (is_sending_) {
    MessageSptr p(response.New());
    p->CopyFrom(response);
    queue_.push(p);
    return true;
  }

  assert(queue_.empty());
  return SendMsg(response);
}

void ServerWriterImpl::SyncClose(const Status& status) {
  AsyncClose(status);
  while (GetQueueSize())
    std::this_thread::yield();
}

void ServerWriterImpl::AsyncClose(const Status& status) {
  Guard g(mtx_);

  if (closed_ || close_status_uptr_)
    return;

  close_status_uptr_.reset(new Status(status));
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

  if (close_status_uptr_) {
    SendStatus();
    return;
  }

  // No message and no close.
  is_sending_ = false;
}

void ServerWriterImpl::SendStatus() {
  assert(!closed_);
  assert(close_status_uptr_);
  assert(queue_.empty());

  closed_ = true;
  const Status& status = *close_status_uptr_;
  using CqTag = ServerWriterSendStatusCqTag;
  CqTag* tag = new CqTag(call_sptr_);
  // Todo: set init md and trail md
  if (tag->StartSend(status, send_init_md_))
    return;
  delete tag;
}

bool ServerWriterImpl::SendMsg(const ::google::protobuf::Message& msg) {
  assert(!closed_);
  assert(!is_sending_);
  is_sending_ = true;

  // CqTag shares this and will TryToWriteNext() on cq completion.
  using CqTag = ServerWriterWriteCqTag;
  CqTag* tag = new CqTag(call_sptr_);
  auto sptr = shared_from_this();
  tag->SetOnComplete([sptr](bool success) {
    // Todo: check success
    sptr->TryToWriteNext();
  });
  if (tag->Start(msg, send_init_md_)) {
    send_init_md_ = false;
    return true;
  }

  delete tag;
  closed_ = true;  // error
  queue_ = MessageQueue();  // reset to break SyncWrite().
  // Todo: do sth. on error?
  return false;
}

}  // namespace grpc_cb
