#!/bin/sh

docker network create \
	--driver=bridge \
	--subnet=192.168.10.0/24 \
	--internal \
	-o "com.docker.network.bridge.name"="bnet-br" \
	-o "com.docker.network.bridge.enable_ip_masquerade"="false" \
	bnet

docker network create \
	--driver=bridge \
	--subnet=10.0.10.0/24 \
	--internal \
	-o "com.docker.network.bridge.name"="unet0-br" \
	-o "com.docker.network.bridge.enable_ip_masquerade"="false" \
	unet0

docker network create \
	--driver=bridge \
	--subnet=10.0.20.0/24 \
	--internal \
	-o "com.docker.network.bridge.name"="unet1-br" \
	-o "com.docker.network.bridge.enable_ip_masquerade"="false" \
	unet1


