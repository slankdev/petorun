
#include <stdio.h>
#include <unistd.h>
#include <dpdk/wrap.h>
#include <slankdev/net/pkt.h>
#include <thread>

void process_one_packet(rte_mbuf* mbuf, size_t pid, size_t qid)
{
  printf("port%zd ", pid);
  uint8_t* ptr = rte_pktmbuf_mtod(mbuf, uint8_t*);
  size_t len = mbuf->pkt_len;
  std::string proto = slankdev::pkt_analyzer::guess_protoname(ptr, len);
  auto vec = slankdev::pkt_analyzer::guess_protostack(ptr, len);
  for (size_t i=0; i<vec.size(); i++) {
    printf("%s%s",
        slankdev::pkt_analyzer::pkt_type2str(vec[i]).c_str(),
        i+1<vec.size()?"/":"\n");
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


