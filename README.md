
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
- KNI manager (using DPDK's API)

**Dependency**
- libslankdev (https://github.com/slankdev/libslankdev)
- libdpdk\_cpp (https://github.com/slankdev/libdpdk\_cpp)

## Usage:

```
$ git clone https://github.com/slankdev/petorun
$ cd petrun/src && make
$ sudo ./petorun -f petorun.conf
```

## Author and Licence

This software is developed under the MIT License.

Author: Hiroki SHIROKURA

- slank.dev [at] gmail.com
- twitter: @slankdev
- facebook: hiroki.shirokura


