// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#include <grpc_cb/impl/call_op_data.h>

#include <grpc_cb/status.h>  // for Status
#include <grpc_cb/support/slice.h>  // for StringFromCopiedSlice()

#include "byte_buffer_to_string.h"  // for ByteBufferToString()

namespace grpc_cb {

Status CodClientRecvStatus::GetStatus() const {
  return Status(status_code_, StringFromCopiedSlice(status_details_));
}

void CodSendMsg::SetMsgStr(const std::string& sMsg)
{
    assert(!send_buf_);
    // send_buf_ is created here and destroyed in dtr().
    grpc_slice slice = SliceFromCopiedString(sMsg);
    send_buf_ = grpc_raw_byte_buffer_create(&slice, 1);
    assert(send_buf_);
    grpc_slice_unref(slice);
}

Status CodRecvMsg::GetResultString(std::string& result) const {
  assert(recv_buf_);
  return ByteBufferToString(*recv_buf_, result);
}

}  // namespace grpc_cb
