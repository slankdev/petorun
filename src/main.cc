
#include <stdio.h>
#include <unistd.h>
#include <dpdk/wrap.h>
#include <thread>

void process_one_packet(rte_mbuf* mbuf, size_t pid, size_t qid)
{
  printf("port%zd -> port%zd \n", pid, pid^1);
  rte_pktmbuf_dump(stdout, mbuf, mbuf->pkt_len);
  printf("\n");

  size_t n_tx = rte_eth_tx_burst(pid^1, qid, &mbuf, 1);
  if (n_tx < 1) rte_pktmbuf_free(mbuf);
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
  port_conf.rxmode.mq_mode = ETH_MQ_RX_RSS;
  port_conf.rx_adv_conf.rss_conf.rss_key = NULL;
  port_conf.rx_adv_conf.rss_conf.rss_hf = ETH_RSS_IP|ETH_RSS_TCP|ETH_RSS_UDP;
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


