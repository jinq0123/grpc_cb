// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CLIENT_CLIENT_ASYNC_READER_HELPER_H
#define GRPC_CB_CLIENT_CLIENT_ASYNC_READER_HELPER_H

#include <atomic>  // for atomic_bool
#include <memory>  // for enable_shared_from_this<>

#include <grpc_cb/impl/call_sptr.h>                              // for CallSptr
#include <grpc_cb/impl/client/client_async_read_handler_sptr.h>  // for ClientAsyncReadHandlerSptr
#include <grpc_cb/impl/completion_queue_sptr.h>  // for CompletionQueueSptr
#include <grpc_cb/status.h>                      // for Status
#include <grpc_cb/status_callback.h>             // for StatusCallback
#include <grpc_cb/support/config.h>              // for GRPC_FINAL

#include "client_async_reader_helper_sptr.h"

namespace grpc_cb {

class ClientReaderAsyncReadCqTag;

// ClientAsyncReaderHelper is used in ClientAsyncReader and ClientAsyncReaderWriter.
class ClientAsyncReaderHelper GRPC_FINAL
    : public std::enable_shared_from_this<ClientAsyncReaderHelper> {
 public:
  ClientAsyncReaderHelper(CompletionQueueSptr cq_sptr, CallSptr call_sptr,
                          std::atomic_bool& is_status_ok,
                          const ClientAsyncReadHandlerSptr& read_handler_sptr,
                          const StatusCallback& on_status);
  ~ClientAsyncReaderHelper();

 public:
  void AsyncReadNext();

 public:
  void OnRead(ClientReaderAsyncReadCqTag& tag);

#if 0
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
#endif

 public:
  CallSptr GetCallSptr() const { return call_sptr_; }

 private:
  const CompletionQueueSptr cq_sptr_;
  const CallSptr call_sptr_;
  std::atomic_bool& is_status_ok_;
  Status status_;

  const ClientAsyncReadHandlerSptr read_handler_sptr_;
  const StatusCallback on_status_;
};  // ClientAsyncReaderHelper

}  // namespace grpc_cb
#endif  // GRPC_CB_CLIENT_CLIENT_ASYNC_READER_HELPER_H
