
#include <thread>
#include <stdio.h>
#include <dpdk/wrap.h>
#include <slankdev/net/addr.h>
#include <slankdev/net/tuntap.h>
#include <slankdev/socketfd.h>


void main_thread(size_t pid, size_t tap)
{
  while (true) {
    rte_mbuf* mbufs[32];
    size_t nrx = rte_eth_rx_burst(pid, 0, mbufs, 32);
    if (nrx != 0) {
      printf("ingress (recv from netif)\n");
      size_t ntx = rte_eth_tx_burst(tap, 0, mbufs, nrx);
      if (ntx < nrx) dpdk::rte_pktmbuf_free_bulk(&mbufs[ntx], nrx-ntx);
    }

    nrx = rte_eth_rx_burst(tap, 0, mbufs, 32);
    if (nrx != 0) {
      printf("egress (recv from tap)\n");
      size_t ntx = rte_eth_tx_burst(pid, 0, mbufs, nrx);
      if (ntx < nrx) dpdk::rte_pktmbuf_free_bulk(&mbufs[ntx], nrx-ntx);
    }
  }
}


int main(int argc, char** argv)
{
  dpdk::dpdk_boot_nopciattach(argc, argv);
  // size_t pid = dpdk::eth_dev_attach("0000:00:08.0");
  size_t pid = dpdk::eth_dev_attach("net_af_packet0,iface=veth0,qpairs=1");
  size_t tap = dpdk::eth_dev_attach("net_tap0,iface=tap0");

  rte_kni_init(1);
  struct rte_eth_conf port_conf;
  struct rte_mempool* mp = dpdk::mp_alloc("slankdev", 0, 8192);

  dpdk::init_portconf(&port_conf);
  port_conf.rxmode.hw_strip_crc = 1;
  dpdk::port_configure(pid, 1, 1, &port_conf, mp);
  dpdk::port_configure(tap, 1, 1, &port_conf, mp);

  std::thread thrd(main_thread, pid, tap);
  slankdev::socketfd::linkup("tap0");
  // slankdev::socketfd::set_ip("tap0", 0xc0a87a02, 24);
  // slankdev::socketfd::set_ip("tap0", slankdev::inetaddr_host(10,0,11,1), 24);
  slankdev::socketfd::set_ip("tap0", 0x0a000b01, 24);
  thrd.join();
}


