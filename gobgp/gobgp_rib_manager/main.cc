
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sstream>
#include <slankdev/hexdump.h>
#include <grpc++/grpc++.h>
#include "gobgp.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReader;
using grpc::Status;
using gobgpapi::GobgpApi;


class gobgp_client {
 public:
  gobgp_client(std::shared_ptr<Channel> channel)
    : stub_(GobgpApi::NewStub(channel)) {}

  void GetPath()
  {
    gobgpapi::GetPathRequest req;
    req.set_family((1<<16) | 1);

    gobgpapi::Path path;
    ClientContext ctx;

    std::unique_ptr< ClientReader<gobgpapi::Path> >
      reader(stub_->GetPath(&ctx, req));
    size_t i = 0;
    while (reader->Read(&path)) {
      printf("[%zd]\n", i);
      printf("- family    : 0x%x \n", path.family());
      printf("- source asn: %u \n", path.source_asn());
      printf("- source id : %s \n", path.source_id().c_str());
      printf("- identifer : %u \n", path.identifier());
      i++;
    }
    Status status = reader->Finish();

    if (!status.ok()) {
      printf("Error \n");
      printf("- error code: %d \n", status.error_code());
      printf("- error msg : %s \n", status.error_message().c_str());
      exit(1);
    }
  }

  void GetNeighbor()
  {
    gobgpapi::GetNeighborRequest  req;
    gobgpapi::GetNeighborResponse res;
    ClientContext ctx;

    Status status = stub_->GetNeighbor(&ctx, req, &res);
    if (!status.ok()) {
      printf("Error \n");
      printf("- error code: %d \n", status.error_code());
      printf("- error msg : %s \n", status.error_message().c_str());
      exit(1);
    }

    std::stringstream buffer;
    for (int i=0; i < res.peers_size(); i++) {
      gobgpapi::PeerConf peer_conf  = res.peers(i).conf();
      gobgpapi::PeerState peer_info = res.peers(i).info();
      gobgpapi::Timers peer_timers  = res.peers(i).timers();

      buffer
        << "BGP neighbor is: " << peer_conf.neighbor_address()
        << ", remote AS: " << peer_conf.peer_as() << "\n"
        << "\tBGP version: 4, remote route ID " << peer_conf.id() << "\n"
        << "\tBGP state = " << peer_info.bgp_state()
        << ", up for " << peer_timers.state().uptime() << "\n"
        << "\tBGP OutQ = " << peer_info.out_q()
        << ", Flops = " << peer_info.flops() << "\n"
        << "\tHold time is " << peer_timers.state().hold_time()
        << ", keepalive interval is " << peer_timers.state().keepalive_interval()
        << "seconds\n"
        << "\tConfigured hold time is " << peer_timers.config().hold_time() << "\n";
    }
    printf("%s", buffer.str().c_str());
  }

  void GetServer()
  {
    gobgpapi::GetServerRequest  req;
    gobgpapi::GetServerResponse res;
    ClientContext ctx;

    Status status = stub_->GetServer(&ctx, req, &res);
    if (!status.ok()) {
      printf("Error \n");
      printf("- error code: %d \n", status.error_code());
      printf("- error msg : %s \n", status.error_message().c_str());
      exit(1);
    }
  }

 private:
  std::unique_ptr<GobgpApi::Stub> stub_;
};

int main(int argc, char** argv)
{
  std::string addr("localhost");
  auto channel = grpc::CreateChannel(addr+":50051",
      grpc::InsecureChannelCredentials());
  gobgp_client client(channel);

  client.GetServer();
  client.GetNeighbor();
  client.GetPath();
}

