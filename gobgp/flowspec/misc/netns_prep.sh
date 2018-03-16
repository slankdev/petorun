#!/bin/sh

ip netns add bgp0
ip netns add usr0
ip netns add usr1

ip link add bgp0eth0 type veth peer name usr0eth0
ip link add bgp0eth1 type veth peer name usr1eth0

ip link set bgp0eth0 netns bgp0
ip link set bgp0eth1 netns bgp0
ip link set usr0eth0 netns usr0
ip link set usr1eth0 netns usr1

ip netns exec bgp0 ip link set lo up
ip netns exec bgp0 ip link set bgp0eth0 up
ip netns exec bgp0 ip link set bgp0eth1 up
ip netns exec bgp0 ethtool -K  bgp0eth0 rx off tx off
ip netns exec bgp0 ethtool -K  bgp0eth1 rx off tx off
# ip netns exec bgp0 ip addr add 10.0.10.1/24    dev bgp0eth0
# ip netns exec bgp0 ip addr add 192.168.10.1/24 dev bgp0eth1

ip netns exec usr0 ip link set lo up
ip netns exec usr0 ip link set usr0eth0 up
ip netns exec usr0 ethtool -K  usr0eth0 rx off tx off
# ip netns exec usr0 ip addr add 10.0.10.2/24 dev em0
# ip netns exec usr0 ip route add default via 10.0.10.1

ip netns exec usr1 ip link set lo up
ip netns exec usr1 ip link set usr1eth0 up
ip netns exec usr1 ethtool -K  usr1eth0 rx off tx off
# ip netns exec usr1 ip addr add 10.0.20.2/24 dev em1
# ip netns exec usr1 ip route add default via 10.0.20.1

ip netns exec bgp0 \
	/usr/lib/quagga/zebra -d \
	-f conf/zebra.conf \
	-i run/zebra.pid \
	-A 127.0.0.1 \
	-z run/zserv.api

