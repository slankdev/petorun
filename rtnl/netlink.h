
#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <net/if.h>
#include <asm/types.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <slankdev/socketfd.h>

#define NETLINK_BUFFER_SIZE 4096

struct iplink_req {
	struct nlmsghdr	n;
	struct ifinfomsg i;
	char buf[1024];
};

struct nlrtmsg {
  struct nlmsghdr hdr;
  struct rtmsg msg;
  uint8_t buf[1024];
};

static const char *
rta_type_name_link (unsigned short rta_type)
{
  switch (rta_type)
    {
    case IFLA_UNSPEC: return "IFLA_UNSPEC";
    case IFLA_ADDRESS: return "IFLA_ADDRESS";
    case IFLA_BROADCAST: return "IFLA_BROADCAST";
    case IFLA_IFNAME: return "IFLA_IFNAME";
    case IFLA_MTU: return "IFLA_MTU";
    case IFLA_LINK: return "IFLA_LINK";
    case IFLA_QDISC: return "IFLA_QDISC";
    case IFLA_STATS: return "IFLA_STATS";

    default: return "unknown";
    }
  return "unknown";
}

static const char *
rta_type_name_addr (unsigned short rta_type)
{
  switch (rta_type)
    {
    case IFA_UNSPEC: return "IFA_UNSPEC";
    case IFA_ADDRESS: return "IFA_ADDRESS";
    case IFA_LOCAL: return "IFA_LOCAL";
    case IFA_LABEL: return "IFA_LABEL";
    case IFA_BROADCAST: return "IFA_BROADCAST";
    case IFA_ANYCAST: return "IFA_ANYCAST";
    case IFA_CACHEINFO: return "IFA_CACHEINFO";

    default: return "unknown";
    }
  return "unknown";
}


void netlink_request_route_dump(int nlsock)
{
  struct nlrtmsg req;

  /* create request message */
  memset (&req, 0, sizeof (req));
  req.hdr.nlmsg_type = RTM_GETROUTE;
  req.hdr.nlmsg_flags = NLM_F_REQUEST | NLM_F_DUMP;
  req.hdr.nlmsg_len = NLMSG_SPACE (sizeof (req.msg));
  req.msg.rtm_family = AF_INET;
  req.msg.rtm_table = RT_TABLE_MAIN;
  req.msg.rtm_protocol = RTPROT_UNSPEC;

  int ret = send (nlsock, &req, req.hdr.nlmsg_len, 0);
  if (ret < 0) {
    printf("fail\n");
    exit(1);
  }
}


void netlink_handle_msg_route(struct nlmsghdr *hdr)
{
  /* verify nlmsg_type */
  if (hdr->nlmsg_type != RTM_NEWROUTE && hdr->nlmsg_type != RTM_DELROUTE) {
    printf("netlink_msg_handle_route() invalid nlmsg_type %d\n",
        hdr->nlmsg_type);
    return;
  }

  /* parse message */
  struct rtmsg* m = (struct rtmsg *) NLMSG_DATA (hdr);
  if (m->rtm_family != AF_INET) {
    printf("is not AF_INET (%d)\n", m->rtm_family);
    return;
  }

#if 0
  if (m->rtm_type == RTN_LOCAL) printf("entry type kernel\n");
  else                          printf("entry type none\n");
#endif

  int attr_len = RTM_PAYLOAD (hdr);
  for (struct rtattr* attr = (struct rtattr *)RTM_RTA(m);
      RTA_OK (attr, attr_len);
      attr = RTA_NEXT (attr, attr_len)) {
    switch (attr->rta_type)
    {
      case RTA_DST:{
        // char buf1[32];
        // memcpy (&rib_target->u.ipv4.prefix, RTA_DATA (attr),
        //         sizeof (rib_target->u.ipv4.prefix));
        // inet_ntop (AF_INET, &(rib_target->u.ipv4.prefix), buf1,
        //            sizeof (buf1));
        break;
      }

      case RTA_GATEWAY: {
        // char buf2[32];
        // rib_entry->u.ipv4.type = rib_entry_type_ipv4;
        // memcpy (&rib_entry->u.ipv4.nexthop, RTA_DATA (attr),
        //         sizeof (rib_entry->u.ipv4.nexthop));
        // inet_ntop (AF_INET, &(rib_entry->u.ipv4.nexthop), buf2,
        //            sizeof (buf2));
        break;
      }

      case RTA_OIF: {
        // check if we need to know about this interface
        int outputif = -1;
        memcpy (&outputif, RTA_DATA (attr), sizeof (outputif));
        break;
      }

        /* todo: we probably need to handle these as well */
      case RTA_PRIORITY:     // route priority
      case RTA_METRICS:      // route metric
      case RTA_MULTIPATH:    // multipath route (ECMP?)
      case RTA_IIF:          // input interface index
        break;

      default:
        break;
    }
  }

#if 1
  if (hdr->nlmsg_type==RTM_NEWROUTE) printf("route add\n");
  else                               printf("route del\n");
#endif
}

inline void netlink_msghdr_dump(FILE* fp,
          const struct nlmsghdr* msghdr)
{
  fprintf(fp, "netlink: message: type: %hu len: %lu, "
          "flags: %#x seq: %lu pid: %lu\n",
          (unsigned short) msghdr->nlmsg_type,
          (unsigned long) msghdr->nlmsg_len,
          (unsigned short) msghdr->nlmsg_flags,
          (unsigned long) msghdr->nlmsg_seq,
          (unsigned long) msghdr->nlmsg_pid);
}

inline void netlink_handle_msg (struct nlmsghdr *msghdr)
{
  /*
   * switch based on nlmsg_type of the message we just received
   * man 7 rtnetlink
   */
  switch (msghdr->nlmsg_type) {

    case RTM_NEWROUTE:
    case RTM_DELROUTE:
      netlink_handle_msg_route(msghdr);
      break;

    case RTM_NEWADDR:
    case RTM_DELADDR:
    case RTM_NEWNEIGH:
    case RTM_DELNEIGH:
    case RTM_NEWLINK:
    case RTM_DELLINK:
      break; // no support

    case NLMSG_DONE : printf("NLMSG_DONE \n"); break;
    case NLMSG_ERROR: printf("NLMSG_ERROR\n"); break;
    default: printf("unknown(%d)\n", msghdr->nlmsg_type); break;
  }
}


void netlink_read_until_done(int nlsock)
{
  char buf[NETLINK_BUFFER_SIZE];
  memset (buf, 0, sizeof (buf));
  char* start = buf;
  int len = 0;

  while (true) {
    size_t recvlen = recv(nlsock, start, sizeof(buf) - len, 0);
    len += recvlen;

    struct nlmsghdr* msghdr = (struct nlmsghdr *) buf;
    for (; NLMSG_OK (msghdr, len); msghdr = NLMSG_NEXT (msghdr, len)) {
        netlink_handle_msg (msghdr);
    }

    if (len == 0) break;
    if (len > 0) {
      printf ("netlink: shifting the data left: %d bytes\n", len);
      memmove(buf, msghdr, len);
      start = buf + len;
    } else start = buf;
  }
}


