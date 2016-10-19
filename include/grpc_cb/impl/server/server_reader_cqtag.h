// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_IMPL_SERVER_SERVER_READER_CQTAG_H
#define GRPC_CB_IMPL_SERVER_SERVER_READER_CQTAG_H

#include <functional>
#include <memory>  // for shared_ptr<>

#include <grpc/support/port_platform.h>    // for GRPC_MUST_USE_RESULT

#include <grpc_cb/impl/call.h>             // for StartBatch()
#include <grpc_cb/impl/call_cqtag.h>       // for CallCqTag
#include <grpc_cb/impl/call_op_data.h>     // for CodSendInitMd
#include <grpc_cb/impl/call_operations.h>  // for CallOperations
#include <grpc_cb/support/config.h>        // for GRPC_FINAL

namespace grpc_cb {

template <class Request, class Response>
class ServerReaderCqTag GRPC_FINAL : public CallCqTag {
 public:
  using Replier = ServerReplier<Response>;
  using MsgCallback = std::function<void (const Request&, const Replier&)>;
  using EndCallback = std::function<void (const Replier&)>;
  struct Data {
    Replier replier;
    MsgCallback on_msg;
    EndCallback on_end;
  };
  using DataSptr = std::shared_ptr<Data>;

 public:
  inline ServerReaderCqTag(const CallSptr& call_sptr,
                           const DataSptr& data_sptr);
  inline bool Start() GRPC_MUST_USE_RESULT;

 public:
  inline void DoComplete(bool success) GRPC_OVERRIDE;

 private:
  CodRecvMsg cod_recv_msg_;
  DataSptr data_sptr_;
};  // class ServerReaderCqTag

template <class Request, class Response>
ServerReaderCqTag<Request, Response>::ServerReaderCqTag(
    const CallSptr& call_sptr, const DataSptr& data_sptr)
    : CallCqTag(call_sptr), data_sptr_(data_sptr) {
  assert(call_sptr);
  assert(data_sptr);
  assert(data_sptr->on_msg);
  assert(data_sptr->on_end);
}

template <class Request, class Response>
bool ServerReaderCqTag<Request, Response>::Start() {
  CallOperations ops;
  ops.RecvMsg(cod_recv_msg_);
  return GetCallSptr()->StartBatch(ops, this);
}

template <class Request, class Response>
void ServerReaderCqTag<Request, Response>::DoComplete(bool success) {
  assert(success);
  if (!cod_recv_msg_.HasGotMsg()) {
    data_sptr_->on_end(data_sptr_->replier);
    return;
  }

  Request request;
  const CallSptr& call_sptr = GetCallSptr();
  Status status = cod_recv_msg_.GetResultMsg(
      request, call_sptr->GetMaxMsgSize());
  if (!status.ok()) {
      data_sptr_->replier.ReplyError(status);
      return;
  }

  data_sptr_->on_msg(request, data_sptr_->replier);

  auto* tag = new ServerReaderCqTag(call_sptr, data_sptr_);
  if (tag->Start()) return;

  delete tag;
  data_sptr_->replier.ReplyError(Status::InternalError(
    "Failed to read client stream."));
}

};  // namespace grpc_cb

#endif  // GRPC_CB_IMPL_SERVER_SERVER_READER_CQTAG_H
