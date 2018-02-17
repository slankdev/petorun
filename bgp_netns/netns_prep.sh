#!/bin/sh

ip netns add bgp0
ip netns add bgp1
ip netns add ns0
ip netns add ns1

ip link add bgp0eth1 type veth peer name bgp1eth1
ip link add bgp0eth0 type veth peer name em0
ip link add bgp1eth0 type veth peer name em1

ip link set bgp0eth0 netns bgp0
ip link set bgp0eth1 netns bgp0

ip link set bgp1eth0 netns bgp1
ip link set bgp1eth1 netns bgp1

ip link set em0 netns ns0
ip link set em1 netns ns1

ip netns exec bgp0 ip link set lo up
ip netns exec bgp0 ip link set bgp0eth0 up
ip netns exec bgp0 ip link set bgp0eth1 up
ip netns exec bgp0 ethtool -K  bgp0eth0 rx off tx off
ip netns exec bgp0 ethtool -K  bgp0eth1 rx off tx off
ip netns exec bgp0 ip addr add 10.0.10.1/24    dev bgp0eth0
ip netns exec bgp0 ip addr add 192.168.10.1/24 dev bgp0eth1

ip netns exec bgp1 ip link set lo up
ip netns exec bgp1 ip link set bgp1eth0 up
ip netns exec bgp1 ip link set bgp1eth1 up
ip netns exec bgp1 ethtool -K  bgp1eth0 rx off tx off
ip netns exec bgp1 ethtool -K  bgp1eth1 rx off tx off
ip netns exec bgp1 ip addr add 10.0.20.1/24    dev bgp1eth0
ip netns exec bgp1 ip addr add 192.168.10.2/24 dev bgp1eth1

ip netns exec ns0 ip link set lo up
ip netns exec ns0 ip link set em0 up
ip netns exec ns0 ethtool -K  em0 rx off tx off
ip netns exec ns0 ip addr add 10.0.10.2/24 dev em0
ip netns exec ns0 ip route add default via 10.0.10.1

ip netns exec ns1 ip link set lo up
ip netns exec ns1 ip link set em1 up
ip netns exec ns1 ethtool -K  em1 rx off tx off
ip netns exec ns1 ip addr add 10.0.20.2/24 dev em1
ip netns exec ns1 ip route add default via 10.0.20.1


