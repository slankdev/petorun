#!/bin/sh

# image=slankdev/ubuntu:16.04
image=devenv
docker run -d --rm --name user0 --privileged -v `pwd`/conf:/root/conf $image ping localhost
docker run -d --rm --name user1 --privileged -v `pwd`/conf:/root/conf $image ping localhost
docker run -d --rm --name bgp0  --privileged -v `pwd`/conf:/root/conf $image ping localhost
docker run -d --rm --name bgp1  --privileged -v `pwd`/conf:/root/conf $image ping localhost

docker network connect bnet  bgp0   --ip=192.168.10.10
docker network connect bnet  bgp1   --ip=192.168.10.20
docker network connect unet0 bgp0  --ip=10.0.10.10
docker network connect unet0 user0 --ip=10.0.10.20
docker network connect unet1 bgp1  --ip=10.0.20.10
docker network connect unet1 user1 --ip=10.0.20.20

sudo ./misc/docker_netns_exec.py user0 "ip r del default"
sudo ./misc/docker_netns_exec.py user0 "ip r add default via 10.0.10.10"
sudo ./misc/docker_netns_exec.py user1 "ip r del default"
sudo ./misc/docker_netns_exec.py user1 "ip r add default via 10.0.20.10"

