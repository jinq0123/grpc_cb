// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CLIENT_CLIENT_READER_ASYNC_READ_CQTAG_H
#define GRPC_CB_CLIENT_CLIENT_READER_ASYNC_READ_CQTAG_H

#include <grpc_cb/support/config.h>   // for GRPC_FINAL
#include <grpc_cb/impl/client/client_reader_read_cqtag.h>  // for ClientReaderReadCqTag

#include "client_async_reader_helper_sptr.h"  // for ClientAsyncReaderHelperSptr

namespace grpc_cb {

// Used in ClientAsyncReaderHelper.
// Todo: rename to ClientAsyncReaderReadCqTag
class ClientReaderAsyncReadCqTag GRPC_FINAL : public ClientReaderReadCqTag {
 public:
  ClientReaderAsyncReadCqTag(const ClientAsyncReaderHelperSptr& reader_sptr);

  void DoComplete(bool success) GRPC_OVERRIDE;

 private:
  const ClientAsyncReaderHelperSptr reader_sptr_;
};  // class ClientReaderAsyncReadCqTag

}  // namespace grpc_cb
#endif  // GRPC_CB_CLIENT_CLIENT_READER_ASYNC_READ_CQTAG_H
