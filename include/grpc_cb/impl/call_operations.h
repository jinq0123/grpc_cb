// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_INTERNAL_CPP_CB_COMMON_CALL_OPERATIONS_H
#define GRPC_INTERNAL_CPP_CB_COMMON_CALL_OPERATIONS_H

#include <cassert>
#include <cstring>  // for memset()
#include <string>

#include <grpc/impl/codegen/grpc_types.h>  // for grpc_op
#include <grpc/support/port_platform.h>    // for GRPC_MUST_USE_RESULT
#include <grpc_cb/impl/call_op_data.h>     // for CodSendInitMd
#include <grpc_cb/impl/metadata_vector.h>  // for MetadataVector
#include <grpc_cb/status.h>                // for Status
#include <grpc_cb/support/config.h>        // for GRPC_FINAL
#include <grpc_cb/support/protobuf_fwd.h>  // for Message
#include <grpc_cb/support/slice.h>         // for SliceFromCopiedString()

struct grpc_slice;

namespace grpc_cb {

// Like grpc++ CallOpSet<>.
// Non-thread-safe.
// Fill grpc_op array for rpc call.
// Only reference to the call operation data, but not the real data,
//   which are kept in CallCqTag, because this object is transient.
class CallOperations GRPC_FINAL {
 public:
  CallOperations() : send_status_details_(grpc_empty_slice()) {
    static_assert(std::is_pod<grpc_op>::value, "grpc_op is not pod.");
    std::memset(ops_, 0, sizeof(ops_));
  }
  ~CallOperations() {
    grpc_slice_unref(send_status_details_);
  }


  inline size_t GetOpsNum() const {
    assert(nops_ <= MAX_OPS);
    return nops_;
  }
  inline const grpc_op* GetOps() const { return ops_; }

 public:
  // Send initial metadata.
  inline void SendInitMd(CodSendInitMd& cod_send_init_md) {
      SendInitMd(cod_send_init_md.GetMdVec());
  }
  inline void SendInitMd(MetadataVector& init_metadata);
  inline Status SendMsg(const ::google::protobuf::Message& message,
                        CodSendMsg& cod_send_msg) GRPC_MUST_USE_RESULT;
  inline void SendMsg(const std::string& message, CodSendMsg& cod_send_msg);

  // Receive initial metadata.
  inline void RecvInitMd(CodRecvInitMd& cod_recv_init_md) {
      RecvInitMd(cod_recv_init_md.GetRecvInitMdArrPtr());
  }
  inline void RecvInitMd(grpc_metadata_array* init_metadata = nullptr);
  inline void RecvMsg(CodRecvMsg& cod_recv_message) {
      RecvMsg(cod_recv_message.GetRecvBufPtr());
  }
  inline void RecvMsg(grpc_byte_buffer** recv_buf);

  inline void ClientSendClose();
  inline void ClientRecvStatus(CodClientRecvStatus& cod_client_recv_status);
  inline void ClientRecvStatus(grpc_metadata_array* trailing_metadata,
                               grpc_status_code* status_code,
                               grpc_slice* status_details);

  inline void ServerSendStatus(const Status& status,
                               CodServerSendStatus& cod_server_send_status);
  inline void ServerSendStatus(grpc_metadata* trail_md, size_t trail_md_count,
                               const grpc_status_code& status_code,
                               const std::string& status_details);

 private:
  inline void SendMsg(const CodSendMsg& cod_send_msg) GRPC_MUST_USE_RESULT;

 private:
  static const size_t MAX_OPS = 8;

  size_t nops_ = 0;
  grpc_op ops_[MAX_OPS];
  grpc_slice send_status_details_;
};  // class CallOperations

static inline void InitOp(grpc_op& op, grpc_op_type type, uint32_t flags = 0) {
  op.op = type;
  op.flags = flags;
  assert(op.reserved == nullptr);
}

// Todo: Set write options.
void CallOperations::SendMsg(const std::string& message,
                             CodSendMsg& cod_send_msg) {
  cod_send_msg.CopyMsgStr(message);
  SendMsg(cod_send_msg);
}

// Todo: Set write options.
Status CallOperations::SendMsg(
    const ::google::protobuf::Message& message,
    CodSendMsg& cod_send_msg) {
  Status status = cod_send_msg.SerializeMsg(message);
  if (!status.ok()) return status;
  SendMsg(cod_send_msg);
  return Status::OK;
}

// Todo: Set write options.
void CallOperations::SendMsg(const CodSendMsg& cod_send_msg) {
  assert(cod_send_msg.GetSendBuf());
  assert(nops_ < MAX_OPS);
  grpc_op& op = ops_[nops_++];
  InitOp(op, GRPC_OP_SEND_MESSAGE);
  op.data.send_message.send_message = cod_send_msg.GetSendBuf();
  // Todo: op->flags = write_options_.flags();
}

void CallOperations::SendInitMd(MetadataVector& init_metadata) {
  assert(nops_ < MAX_OPS);
  grpc_op& op = ops_[nops_++];
  InitOp(op, GRPC_OP_SEND_INITIAL_METADATA);
  op.data.send_initial_metadata.count = init_metadata.size();
  op.data.send_initial_metadata.metadata =
      init_metadata.empty() ? nullptr : &init_metadata[0];
}

void CallOperations::RecvInitMd(grpc_metadata_array* init_metadata) {
  assert(nops_ < MAX_OPS);
  grpc_op& op = ops_[nops_++];
  InitOp(op, GRPC_OP_RECV_INITIAL_METADATA);  // Todo
  op.data.recv_initial_metadata.recv_initial_metadata = init_metadata;
}

void CallOperations::RecvMsg(grpc_byte_buffer** recv_buf) {
  assert(nops_ < MAX_OPS);
  grpc_op& op = ops_[nops_++];
  InitOp(op, GRPC_OP_RECV_MESSAGE);
  op.data.recv_message.recv_message = recv_buf;
}

void CallOperations::ClientSendClose() {
  assert(nops_ < MAX_OPS);
  grpc_op& op = ops_[nops_++];
  InitOp(op, GRPC_OP_SEND_CLOSE_FROM_CLIENT);
}

void CallOperations::ClientRecvStatus(CodClientRecvStatus& cod) {
  ClientRecvStatus(cod.GetTrailMdArrPtr(), cod.GetStatusCodePtr(),
                   cod.GetStatusDetailsPtr());
}

void CallOperations::ClientRecvStatus(grpc_metadata_array* trailing_metadata,
                                      grpc_status_code* status_code,
                                      grpc_slice* status_details) {
  assert(nops_ < MAX_OPS);
  grpc_op& op = ops_[nops_++];
  InitOp(op, GRPC_OP_RECV_STATUS_ON_CLIENT);
  op.data.recv_status_on_client.trailing_metadata = trailing_metadata;
  op.data.recv_status_on_client.status = status_code;
  op.data.recv_status_on_client.status_details = status_details;
}

void CallOperations::ServerSendStatus(const Status& status,
                               CodServerSendStatus& cod) {
  cod.SetStatus(status);  // Must save in Cod to live until completion.
  ServerSendStatus(cod.GetTrailMdArrPtr(), cod.GetTrailMdCount(),
      cod.GetStatusCode(), cod.GetStatusDetailsBuf());
}

void CallOperations::ServerSendStatus(
    grpc_metadata* trail_md, size_t trail_md_count,
    const grpc_status_code& status_code, const std::string& status_details) {
  assert(nops_ < MAX_OPS);
  grpc_op& op = ops_[nops_++];
  InitOp(op, GRPC_OP_SEND_STATUS_FROM_SERVER);
  op.data.send_status_from_server.trailing_metadata_count = trail_md_count,
  op.data.send_status_from_server.trailing_metadata = trail_md;
  op.data.send_status_from_server.status = status_code;
  send_status_details_ = SliceFromCopiedString(status_details);
  op.data.send_status_from_server.status_details = &send_status_details_;
}

}  // namespace grpb_cb

#endif  // GRPC_INTERNAL_CPP_CB_COMMON_CALL_OPERATIONS_H
