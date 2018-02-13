
# petorun

This is prototype software router implementation.
This is always under the development because of testing protocol.

## Usage:

```
$ git clone https://github.com/slankdev/petorun
$ cd petrun/src && make
$ ../misc/netns_prep.sh
$ sudo ./petorun
$ ../misc/netns_clean.sh
```

This misc script creates 2 network-namespace for debug.
like following AA..

```
+-----------+       +----------------+       +-----------+
|           |       |                |       |           |
| ns0 <ns0eth0>---<veth0> Petrun <veth1>---<ns1eth0> ns1 |
|           |       |                |       |           |
+-----------+       +----------------+       +-----------+
```


