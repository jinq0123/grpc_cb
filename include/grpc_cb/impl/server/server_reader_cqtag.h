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

template <class MsgType>
class ServerReaderCqTag GRPC_FINAL : public CallCqTag {
 public:
  using Reader = ServerReader<MsgType>;
  using ReaderSptr = std::shared_ptr<Reader>;
  inline ServerReaderCqTag(const CallSptr& call_sptr,
                           const ReaderSptr& reader_sptr);
 public:
  inline bool Start() GRPC_MUST_USE_RESULT;
 public:
  inline void DoComplete(bool success) GRPC_OVERRIDE;
 private:
  CodRecvMsg cod_recv_msg_;
  ReaderSptr reader_sptr_;  // may be null
};  // class ServerReaderCqTag

template <class MsgType>
ServerReaderCqTag<MsgType>::ServerReaderCqTag(
    const CallSptr& call_sptr, const ReaderSptr& reader_sptr)
    : CallCqTag(call_sptr), reader_sptr_(reader_sptr) {
  assert(call_sptr);
  assert(reader_sptr);
}

template <class MsgType>
bool ServerReaderCqTag<MsgType>::Start() {
  CallOperations ops;
  ops.RecvMsg(cod_recv_msg_);
  return GetCallSptr()->StartBatch(ops, this);
}

template <class MsgType>
void ServerReaderCqTag<MsgType>::DoComplete(bool success) {
  assert(success);
  if (!cod_recv_msg_.HasGotMsg()) {
    reader_sptr_->OnEnd();
    return;
  }

  MsgType msg;
  const CallSptr& call_sptr = GetCallSptr();
  Status status = cod_recv_msg_.GetResultMsg(
      msg, call_sptr->GetMaxMsgSize());
  if (!status.ok()) {
    reader_sptr_->OnError(status);
    return;
  }
  reader_sptr_->OnMsg(msg);

  auto* tag = new ServerReaderCqTag(call_sptr, reader_sptr_);
  if (tag->Start()) return;
  delete tag;
  reader_sptr_->OnError(Status::InternalError(
    "Failed to read client stream."));
}

}  // namespace grpc_cb

#endif  // GRPC_CB_IMPL_SERVER_SERVER_READER_CQTAG_H
