/*
 *
 * Copyright 2015, Google Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

// Generates cpp_cb gRPC service interface out of Protobuf IDL.
//

#include <memory>

#include "config.h"
#include "cpp_generator_helpers.h"

#include "cpp_cb_generator.h"

class CppcbGrpcGenerator : public grpc::protobuf::compiler::CodeGenerator {
 public:
  CppcbGrpcGenerator() {}
  virtual ~CppcbGrpcGenerator() {}

  virtual bool Generate(const grpc::protobuf::FileDescriptor *file,
                        const grpc::string &parameter,
                        grpc::protobuf::compiler::GeneratorContext *context,
                        grpc::string *error) const {
    if (file->options().cc_generic_services()) {
      *error =
          "cpp grpc proto compiler plugin does not work with generic "
          "services. To generate cpp grpc APIs, please set \""
          "cc_generic_service = false\".";
      return false;
    }

    grpc_cpp_cb_generator::Parameters generator_parameters;

    if (!parameter.empty()) {
      std::vector<grpc::string> parameters_list =
        grpc_generator::tokenize(parameter, ",");
      for (auto parameter_string = parameters_list.begin();
           parameter_string != parameters_list.end();
           parameter_string++) {
        std::vector<grpc::string> param =
          grpc_generator::tokenize(*parameter_string, "=");
        if (param[0] == "services_namespace") {
          generator_parameters.services_namespace = param[1];
        } else {
          *error = grpc::string("Unknown parameter: ") + *parameter_string;
          return false;
        }
      }
    }

    grpc::string file_name = grpc_generator::StripProto(file->name());

    grpc::string header_code =
        grpc_cpp_cb_generator::GetHeaderPrologue(file, generator_parameters) +
        grpc_cpp_cb_generator::GetHeaderIncludes(file, generator_parameters) +
        grpc_cpp_cb_generator::GetHeaderServices(file, generator_parameters) +
        grpc_cpp_cb_generator::GetHeaderEpilogue(file, generator_parameters);
    std::unique_ptr<grpc::protobuf::io::ZeroCopyOutputStream> header_output(
        context->Open(file_name + ".grpc_cb.pb.h"));
    grpc::protobuf::io::CodedOutputStream header_coded_out(
        header_output.get());
    header_coded_out.WriteRaw(header_code.data(), header_code.size());

    grpc::string source_code =
        grpc_cpp_cb_generator::GetSourcePrologue(file, generator_parameters) +
        grpc_cpp_cb_generator::GetSourceIncludes(file, generator_parameters) +
        grpc_cpp_cb_generator::GetSourceDescriptors(file, generator_parameters) +
        grpc_cpp_cb_generator::GetSourceServices(file, generator_parameters) +
        grpc_cpp_cb_generator::GetSourceEpilogue(file, generator_parameters);
    std::unique_ptr<grpc::protobuf::io::ZeroCopyOutputStream> source_output(
        context->Open(file_name + ".grpc_cb.pb.cc"));
    grpc::protobuf::io::CodedOutputStream source_coded_out(
        source_output.get());
    source_coded_out.WriteRaw(source_code.data(), source_code.size());

    return true;
  }
};

int main(int argc, char *argv[]) {
  CppcbGrpcGenerator generator;
  return grpc::protobuf::compiler::PluginMain(argc, argv, &generator);
}
