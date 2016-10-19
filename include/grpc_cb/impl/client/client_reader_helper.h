// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_IMPL_CLIENT_CLIENT_READER_HELPER_H
#define GRPC_CB_IMPL_CLIENT_CLIENT_READER_HELPER_H

#include <cassert>     // for assert()
#include <functional>  // for std::function

#include <grpc_cb/impl/call_sptr.h>                              // for CallSptr
#include <grpc_cb/impl/client/client_reader_async_read_cqtag.h>  // for ClientReaderAsyncReadCqTag
#include <grpc_cb/impl/client/client_reader_async_recv_status_cqtag.h>  // for ClientReaderAsyncRecvStatusCqTag
#include <grpc_cb/impl/client/client_reader_data.h>  // for ClientReaderDataSptr
#include <grpc_cb/impl/client/client_reader_read_cqtag.h>  // for ClientReaderReadCqTag
#include <grpc_cb/impl/client/client_reader_recv_status_cqtag.h>  // for ClientReaderRecvStatusCqTag
#include <grpc_cb/impl/completion_queue.h>  // for CompletionQueue::Pluck()
#include <grpc_cb/status.h>                 // for Status
#include <grpc_cb/status_callback.h>        // for StatusCallback

namespace grpc_cb {
namespace ClientReaderHelper {
// ClientReaderHelper is used in ClientReader and ClientReaderWriter.

inline bool BlockingReadOne(
    const CallSptr& call_sptr,
    const CompletionQueueSptr& cq_sptr,
    ::google::protobuf::Message& response,
    Status& status);

inline Status BlockingRecvStatus(
    const CallSptr& call_sptr,
    const CompletionQueueSptr& cq_sptr);

// Callback on each message.
template <class Response>
inline void OnReadEach(const Response& msg,
    const ClientReaderDataSptr<Response>& data_sptr);

// Callback on end of reading or by error.
template <class Response>
inline void OnEnd(const Status& status,
    const ClientReaderDataSptr<Response>& data_sptr);

// Setup next async read.
template <class Response>
inline void AsyncReadNext(const ClientReaderDataSptr<Response>& data_sptr);

inline void AsyncRecvStatus(
    const CallSptr& call_sptr,
    Status& status,
    const StatusCallback& on_status);

// Todo: move to cpp file.

inline bool BlockingReadOne(
    const CallSptr& call_sptr,
    const CompletionQueueSptr& cq_sptr,
    ::google::protobuf::Message& response,
    Status& status) {
  if (!status.ok()) return false;

  ClientReaderReadCqTag tag(call_sptr);
  if (!tag.Start()) {
    status.SetInternalError("End of server stream.");  // Todo: use EndOfStream instead of status.
    return false;
  }

  // tag.Start() has queued the tag. Wait for completion.
  cq_sptr->Pluck(&tag);
  // Todo: check HasGotMsg()...
  status = tag.GetResultMsg(response);
  return status.ok();
}

inline Status BlockingRecvStatus(
    const CallSptr& call_sptr,
    const CompletionQueueSptr& cq_sptr) {
  assert(call_sptr);
  assert(cq_sptr);
  ClientReaderRecvStatusCqTag tag(call_sptr);
  if (!tag.Start())
      return Status::InternalError("Failed to receive status.");
  cq_sptr->Pluck(&tag);
  return tag.GetStatus();
}

template <class Response>
inline void AsyncReadNext(const ClientReaderDataSptr<Response>& data_sptr) {
  assert(data_sptr);
  Status& status = data_sptr->status;
  if (!status.ok()) return;

  auto* tag = new ClientReaderAsyncReadCqTag<Response>(
      data_sptr->call_sptr,
      [data_sptr](const Response& msg) { OnReadEach(msg, data_sptr); },
      [data_sptr](const Status& status) { OnEnd(status, data_sptr); });
  if (tag->Start()) return;

  delete tag;
  status.SetInternalError("Failed to async read server stream.");
  StatusCallback& on_status = data_sptr->on_status;
  if (on_status) on_status(status);
}

inline void AsyncRecvStatus(
    const CallSptr& call_sptr,
    Status& status,
    const StatusCallback& on_status) {
  assert(status.ok());

  auto* tag = new ClientReaderAsyncRecvStatusCqTag(call_sptr, on_status);
  if (tag->Start()) return;

  delete tag;
  status.SetInternalError("Failed to receive status.");
  if (on_status) on_status(status);
}

template <class Response>
inline void OnReadEach(const Response& msg,
    const ClientReaderDataSptr<Response>& data_sptr) {
  Status& status = data_sptr->status;
  assert(status.ok());

  std::function<void(const Response&)>& on_msg = data_sptr->on_msg;
  if (on_msg) on_msg(msg);

  AsyncReadNext(data_sptr);
  // Old tag will be deleted after return in BlockingRun().
}

template <class Response>
inline void OnEnd(const Status& status,
    const ClientReaderDataSptr<Response>& data_sptr) {
  StatusCallback& on_status = data_sptr->on_status;
  if (status.ok()) {
    AsyncRecvStatus(data_sptr->call_sptr,
        data_sptr->status, on_status);
    return;
  }

  if (on_status) on_status(status);
}

}  // namespace ClientReaderHelper
}  // namespace grpc_cb

#endif  // GRPC_CB_IMPL_CLIENT_CLIENT_READER_HELPER_H
