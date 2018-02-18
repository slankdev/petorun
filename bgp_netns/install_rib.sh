#!/bin/sh

ip netns exec bgp0 /home/vagrant/go/bin/gobgp global rib add 10.0.10.0/24
ip netns exec bgp1 /home/vagrant/go/bin/gobgp global rib add 10.0.20.0/24


