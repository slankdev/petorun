#!/bin/sh

ip netns exec bgp0 \
	/usr/lib/quagga/zebra -d \
	-f conf/zebra.conf \
	-i /var/run/quagga/zebra0.pid \
	-A 127.0.0.1 \
	-z /var/run/quagga/zebra0.vty

