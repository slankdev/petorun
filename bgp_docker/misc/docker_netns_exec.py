#!/usr/bin/env python3
import sys
import docker
import subprocess

def main():
    if len(sys.argv) < 3:
        print('Usage: {} container-name cmd'.format(sys.argv[0]))
        exit(1)

    name = sys.argv[1]
    cmd  = sys.argv[2]

    client = docker.from_env()
    container = client.containers.get(name)
    pid = container.attrs['State']['Pid']

    contnetpath = '/proc/{}/ns/net'.format(pid)
    netnspath = '/var/run/netns/{}'.format(name)
    subprocess.call('ln -s {} {}'.format(contnetpath, netnspath).split())
    subprocess.call('ip netns exec {} {}'.format(name, cmd).split())
    subprocess.call('ip netns del {}'.format(name).split())

if __name__ == '__main__':
    main()

