
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

int netlink_handle_ifaddr_msg (struct nlmsghdr *hdr)
{
  struct ifaddrmsg *m;
  struct rtattr *attr;
  int attr_len, ifindex, plen;
  uint32_t addr;
  char buf[32];
  struct tap_port *port;

  /* verify nlmsg_type */
  if (hdr->nlmsg_type != RTM_NEWADDR && hdr->nlmsg_type != RTM_DELADDR)
    {
      printf ("netlink_handle_ifaddr_msg(): invalid nlmsg_type %d\n",
              hdr->nlmsg_type);
      return -1;
    }

  /* parse message */
  m = (struct ifaddrmsg *) NLMSG_DATA (hdr);
  ifindex = m->ifa_index;
  plen = m->ifa_prefixlen;

  // only interested in ipv4 for now
  if (m->ifa_family != AF_INET)
    {
      printf ("netlink_handle_ifaddr_msg(): not AF_INET (%d)\n",
              m->ifa_family);
      return 0;
    }

  attr_len = IFA_PAYLOAD (hdr);
  for (attr = (struct rtattr *) IFA_RTA (m); RTA_OK (attr, attr_len);
       attr = RTA_NEXT (attr, attr_len))
    {
      printf ("netlink: ifaddrmsg: rta_type %d: %s\n",
              attr->rta_type, rta_type_name_addr (attr->rta_type));

      switch (attr->rta_type)
        {
        case IFA_LOCAL:
          // make sure it's AF_INET..
          memcpy (&addr, RTA_DATA (attr), sizeof (addr));
          inet_ntop (AF_INET, &addr, buf, sizeof (buf));
          break;

          /* todo: we probably need to handle below as well */
        case IFA_ADDRESS:      // network address
          // if point-to-point link (or /31) this becomes destination address
          // we need to check m->ifa_flags for FlagPointToPoint
          memcpy (&addr, RTA_DATA (attr), sizeof (addr));
          inet_ntop (AF_INET, &addr, buf, sizeof (buf));
          break;

        case IFA_BROADCAST:    // broadcast address
          //break;

        default:
          break;
        }
    }

  /* notify rib_manager */
  // rib_ifaddr->ipaddr.s_addr = addr;
  // rib_ifaddr->prefix_len = plen;
  // rib_msg->type = rib_msg_type_ipv4_ifaddr_assign;
  // printf ("netlink: notify ifaddr to rib_manager: "
  //         "type %d transaction_id %d ifindex %d ipaddr %s/%d\n",
  //         rib_msg->type, rib_msg->transaction_id, ifindex, buf, plen);
  return 0;
}

void netlink_handle_route_msg(struct nlmsghdr *hdr)
{
  struct rtmsg *m;
  struct rtattr *attr;
  char buf1[32], buf2[32], *rib_target_entry;
  int attr_len, size, outputif = -1;

  /* verify nlmsg_type */
  if (hdr->nlmsg_type != RTM_NEWROUTE && hdr->nlmsg_type != RTM_DELROUTE)
    {
      printf ("netlink_msg_handle_route() invalid nlmsg_type %d\n",
              hdr->nlmsg_type);
      return;
    }

  /* parse message */
  m = (struct rtmsg *) NLMSG_DATA (hdr);
  if (m->rtm_family != AF_INET) {
    // ignore AF_INET6 and others
    printf ("netlink_msg_handle_route() not AF_INET (%d)\n",
        m->rtm_family);
    return;
  }

#if 0
  if (m->rtm_type == RTN_LOCAL) printf("entry type kernel\n");
  else                          printf("entry type none\n");
#endif

  attr_len = RTM_PAYLOAD (hdr);
  for (attr = (struct rtattr *) RTM_RTA (m);
       RTA_OK (attr, attr_len);
       attr = RTA_NEXT (attr, attr_len)) {
      switch (attr->rta_type)
        {
        case RTA_DST:
          // memcpy (&rib_target->u.ipv4.prefix, RTA_DATA (attr),
          //         sizeof (rib_target->u.ipv4.prefix));
          // inet_ntop (AF_INET, &(rib_target->u.ipv4.prefix), buf1,
          //            sizeof (buf1));
          break;

        case RTA_GATEWAY:
          // rib_entry->u.ipv4.type = rib_entry_type_ipv4;
          // memcpy (&rib_entry->u.ipv4.nexthop, RTA_DATA (attr),
          //         sizeof (rib_entry->u.ipv4.nexthop));
          // inet_ntop (AF_INET, &(rib_entry->u.ipv4.nexthop), buf2,
          //            sizeof (buf2));
          break;

        case RTA_OIF:          // check if we need to know about this interface
          memcpy (&outputif, RTA_DATA (attr), sizeof (outputif));
          break;

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

#if 0
  if (hdr->nlmsg_type==RTM_NEWROUTE) printf("route add\n");
  else                               printf("route del\n");
#endif
}


int netlink_handle_msg (struct nlmsghdr *msghdr)
{
#if 0
  printf ("netlink: message: type: %hu len: %lu, "
          "flags: %#x seq: %lu pid: %lu\n",
          (unsigned short) msghdr->nlmsg_type,
          (unsigned long) msghdr->nlmsg_len,
          (unsigned short) msghdr->nlmsg_flags,
          (unsigned long) msghdr->nlmsg_seq,
          (unsigned long) msghdr->nlmsg_pid);
#endif
  /*
   * switch based on nlmsg_type of the message we just received
   * man 7 rtnetlink
   */
  switch (msghdr->nlmsg_type) {
    case RTM_NEWADDR:
    case RTM_DELADDR:
      netlink_handle_ifaddr_msg(msghdr);
      break;

    case RTM_NEWROUTE:
    case RTM_DELROUTE:
      netlink_handle_route_msg(msghdr);
      break;

    case NLMSG_DONE:
    case RTM_NEWNEIGH:
    case RTM_DELNEIGH:
    case RTM_NEWLINK:
    case RTM_DELLINK:
      break; // no support

    case NLMSG_ERROR:
    default:
      printf("type=%d: unknown or error\n", msghdr->nlmsg_type);
      break;
  }
}

void netlink_read_until_done(int nlsock)
{
  char buf[NETLINK_BUFFER_SIZE];
  int len, ret, err;
  struct nlmsghdr *msghdr;
  char *start;
  int exitflag = 0;

  memset (buf, 0, sizeof (buf));
  start = buf;
  len = 0;

  while (true) {
    ret = recv (nlsock, start, sizeof (buf) - len, 0);
    if (ret < 0) continue;

    len += ret;
    for (msghdr = (struct nlmsghdr *) buf; NLMSG_OK (msghdr, len);
         msghdr = NLMSG_NEXT (msghdr, len)) {

        err = netlink_handle_msg (msghdr);
        if (err < 0)
          printf ("netlink: message handler returned error: %d\n", err);

        if (msghdr->nlmsg_type == NLMSG_DONE ||
            msghdr->nlmsg_type == NLMSG_ERROR) {
          printf("netlink_read_until_done() "
                 "finish code %d\n",
                 msghdr->nlmsg_type);
          exitflag++;
        }
    }

    if (exitflag && len == 0)
      break;

    if (len > 0)
      {
        printf ("netlink: shifting the data left: %d bytes\n", len);
        memmove (buf, msghdr, len);
        start = buf + len;
      }
    else
      start = buf;
  }
}
