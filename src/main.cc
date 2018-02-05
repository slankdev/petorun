
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <dpdk/wrap.h>
#include <slankdev/net/pkt.h>
#include <thread>

inline void fix_udp_checksum(uint8_t* ptr, size_t len)
{
  using namespace slankdev;
  constexpr auto pt_eth  = pkt_analyzer::proto_eth;
  constexpr auto pt_ipv4 = pkt_analyzer::proto_ipv4;
  constexpr auto pt_udp  = pkt_analyzer::proto_udp;

  size_t dis_ih = pkt_analyzer::find_hdr_distance(ptr, len, pt_eth, pt_ipv4);
  ip*  ih = (ip* )(ptr + dis_ih);
  size_t dis_uh = pkt_analyzer::find_hdr_distance(ih, len, pt_ipv4, pt_udp);
  udp* uh = (udp*)((uint8_t*)ih + dis_uh);
  uh->cksum = 0;
  uh->cksum = ipv4_tcpudp_checksum(ih, uh);
}

inline void process_one_packet(rte_mbuf* mbuf, size_t pid, size_t qid)
{
  using namespace slankdev;
  uint8_t* ptr = rte_pktmbuf_mtod(mbuf, uint8_t*);
  size_t len = mbuf->pkt_len;
  std::string proto = pkt_analyzer::guess_protoname(ptr, len);
  if (proto=="udp") {
    for (size_t i=0; i<len-5; i++) {
      char* strp = reinterpret_cast<char*>(&ptr[i]);
      if (strncmp(strp, "latex", 5) == 0) {
        printf("port%zd ", pid);
        printf("latex -> stysf\n");
        memcpy(strp, "stysf", strlen("stysf"));
      }
      fix_udp_checksum(ptr, len);
    }
  }

  size_t n_tx = rte_eth_tx_burst(pid^1, qid, &mbuf, 1);
  if (n_tx < 1) {
    printf("Dropped\n");
    rte_pktmbuf_free(mbuf);
  }
}

constexpr size_t n_queues = 1;
int main_thread(void*)
{
  const size_t n_ports = rte_eth_dev_count();
  while (true) {
    for (size_t pid=0; pid<n_ports; pid++) {
			for (size_t qid=0; qid<n_queues; qid++) {
				constexpr size_t BURSTSZ = 32;
				rte_mbuf* mbufs[BURSTSZ];
				size_t n_rx = rte_eth_rx_burst(pid, qid, mbufs, BURSTSZ);
				if (n_rx == 0) continue;
        for (size_t i=0; i<n_rx; i++)
          process_one_packet(mbufs[i], pid, qid);
			}
    }
  }
}

int main(int argc, char** argv)
{
  dpdk::dpdk_boot(argc, argv);
  struct rte_eth_conf port_conf;
  dpdk::init_portconf(&port_conf);
  struct rte_mempool* mp = dpdk::mp_alloc("RXMBUFMP", 0, 8192);

  size_t n_ports = rte_eth_dev_count();
  if (n_ports == 0) throw dpdk::exception("no ethdev");
  printf("%zd ports found \n", n_ports);
  for (size_t i=0; i<n_ports; i++) {
    dpdk::port_configure(i, n_queues, n_queues, &port_conf, mp);
  }

  std::thread thrd(main_thread, nullptr);
  thrd.join();
}


