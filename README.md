
# Petorun

Petorun is yet another software router implementation.
(under the development)

**Design Principle**
- Moduler: developer can extend new feature easily
- NFV-Aware: Good at combination with VM/Container
- SDN-Aware: Easy to operatewith software

**Sub-systems**
- Front End (using Openconfigd via gRPC)
- RIB manager (connect to gobgpd via gRPC)
- KNI manager for native linux-interface (using DPDK's API)
- TAP manager for native linux-interface (using DPDK's API)

**Dependency**
- libslankdev (https://github.com/slankdev/libslankdev)
- libdpdk\_cpp (https://github.com/slankdev/libdpdk\_cpp)

## Usage:

```
$ git clone https://github.com/slankdev/petorun
$ cd petrun/src && make
$ cat petorun.conf
{
  "dplane" : {
    "n_port" : 2,
    "ports" : [
      {
        "ifname": "petorun0",
        "pcidev": "0000:3b:00.0",
      },
      {
        "ifname": "petorun1",
        "pcidev": "0000:3b:00.1",
      }
    ]
  },
  "cplane" : {
    "daemon": "gobgp"
    "host": "localhost:59001"
  }
}
$ sudo ./petorun -f petorun.conf
```

## Author and Licence

This software is developed under the MIT License.

Author: Hiroki SHIROKURA

- slank.dev [at] gmail.com
- twitter: @slankdev
- facebook: hiroki.shirokura


