#!/bin/sh

ip netns add user
ip link add veth0 type veth peer name veth1
ip link set veth1 netns user

ip netns exec user ip link set lo up
ip netns exec user ip link set veth1 up
ip netns exec user ethtool -K  veth1 rx off tx off
ip netns exec user ip addr add 10.0.11.2/24 dev veth1

