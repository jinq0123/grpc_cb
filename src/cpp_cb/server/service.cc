// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#include <grpc_cb/server/service.h>

#include <google/protobuf/descriptor.h>

namespace grpc_cb {

const std::string& Service::GetFullName() const {
  return GetDescriptor().full_name();
}

size_t Service::GetMethodCount() const {
  return GetDescriptor().method_count();
}

bool Service::IsMethodClientStreaming(size_t method_index) const {
  assert(method_index < GetMethodCount());
  const ::google::protobuf::ServiceDescriptor& svc_desc = GetDescriptor();
  const ::google::protobuf::MethodDescriptor* method_desc =
    svc_desc.method(method_index);
  assert(method_desc);
  return method_desc->client_streaming();
}

}  // namespace grpc_cb
