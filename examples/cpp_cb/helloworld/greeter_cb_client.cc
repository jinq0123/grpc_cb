#include <iostream>
#include <memory>
#include <string>

#include <grpc_cb/grpc_cb.h>

#include "helloworld.grpc_cb.pb.h"

using grpc_cb::Channel;
using grpc_cb::ChannelSptr;
using grpc_cb::Status;

using helloworld::HelloRequest;
using helloworld::HelloReply;
namespace Greeter = helloworld::Greeter;

class GreeterClient {
 public:
  GreeterClient(ChannelSptr channel)
      : stub_(new Greeter::Stub(channel)) {}

  // Assambles the client's payload, sends it and presents the response back
  // from the server.
  std::string SyncSayHello(const std::string& user) {
    // Data we are sending to the server.
    HelloRequest request;
    request.set_name(user);

    // Container for the data we expect from the server.
    HelloReply reply;

    // The actual RPC.
    Status status = stub_->Sync_SayHello(request, &reply);

    // Act upon its status.
    if (status.ok()) {
      return reply.message();
    } else {
      return "RPC failed: " + status.GetDetails();
    }
  }

 private:
  std::unique_ptr<Greeter::Stub> stub_;
};

int main(int argc, char** argv) {
  ChannelSptr channel(new Channel("localhost:50051"));
  GreeterClient greeter(channel);
  std::string user("world");
  std::string reply = greeter.SyncSayHello(user);
  std::cout << "Greeter received: " << reply << std::endl;

  // Async client.
  helloworld::Greeter::Stub stub(channel);
  HelloRequest request;
  request.set_name("async_world");
  stub.Async_SayHello(request, [](const helloworld::HelloReply& resp) {
      std::cout << "Async greeter received: " << resp.message() << std::endl;
    });
  stub.Run();
  return 0;
}
