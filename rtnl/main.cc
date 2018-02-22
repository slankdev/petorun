
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "netlink.h"

int main(int argc, char** argv)
{
#if 1
  slankdev::netlink netlink;
  netlink.dump_route();
  // netlink.add_route();
  // netlink.del_route();
#else
  slankdev::socketfd sock;
	sock.socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
  int nlsock = sock.get_fd();
  netlink_request_route_dump(nlsock);
  netlink_read_until_done(nlsock);
#endif
}


