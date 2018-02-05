#!/bin/sh

ip netns add ns0
ip netns add ns1

ip link add veth0 type veth peer name ns0eth0
ip link add veth1 type veth peer name ns1eth0
ip link set ns0eth0 netns ns0
ip link set ns1eth0 netns ns1

ip netns exec ns0 ip link set lo up
ip netns exec ns0 ip link set ns0eth0 up
ip netns exec ns0 ethtool -K ns0eth0 rx off tx off

ip netns exec ns1 ip link set lo up
ip netns exec ns1 ip link set ns1eth0 up
ip netns exec ns1 ethtool -K ns1eth0 rx off tx off

ip link set veth0 up
ip link set veth1 up


