#!/bin/sh

ip netns exec bgp1 \
	/usr/lib/quagga/zebra -d \
	-f conf/zebra.conf \
	-i /var/run/quagga/zebra1.pid \
	-A 127.0.0.1 \
	-z /var/run/quagga/zebra1.vty

