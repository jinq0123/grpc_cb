// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)
#ifndef GRPC_CB_SERVER_SERVICE_H
#define GRPC_CB_SERVER_SERVICE_H

#include <string>

#include <grpc_cb_core/server/service.h>  // for grpc_cb_core::Service

#include <grpc_cb/common/impl/grpc_cb_api.h>  // for GRPC_CB_API
#include <grpc_cb/common/impl/protobuf_fwd.h>  // for ServiceDescriptor
#include <grpc_cb/common/impl/config.h>   // for GRPC_OVERRIDE

struct grpc_byte_buffer;

namespace grpc_cb {

// Service base class.
class GRPC_CB_API Service : public grpc_cb_core::Service {
 public:
  const std::string& GetFullName() const GRPC_OVERRIDE;
  size_t GetMethodCount() const GRPC_OVERRIDE;
  bool IsMethodClientStreaming(size_t method_index) const GRPC_OVERRIDE;

 private:
  virtual const ::google::protobuf::ServiceDescriptor& GetDescriptor()
      const = 0;
};  // class Service

}  // namespace grpc_cb
#endif  // GRPC_CB_SERVER_SERVICE_H
