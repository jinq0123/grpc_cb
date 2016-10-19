// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#include <grpc_cb/service.h>

#include <google/protobuf/descriptor.h>

namespace grpc_cb {

const std::string& Service::GetFullName() const {
  return GetDescriptor().full_name();
}

size_t Service::GetMethodCount() const {
  return GetDescriptor().method_count();
}

}  // namespace grpc_cb
