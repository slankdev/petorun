#!/bin/sh

misc/netns_prep.sh
misc/zebra0_prep.sh
misc/zebra1_prep.sh

gobgpd=/home/vagrant/go/bin/gobgpd
ip netns exec bgp0 $gobgpd -p -f conf/gobgpd0.conf &
ip netns exec bgp1 $gobgpd -p -f conf/gobgpd1.conf &

sleep 2

gobgp=/home/vagrant/go/bin/gobgp
ip netns exec bgp0 $gobgp global rib add 10.0.10.0/24
ip netns exec bgp1 $gobgp global rib add 10.0.20.0/24


