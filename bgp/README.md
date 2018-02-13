
# Leaning BGP with GoBGP and NETNS

This is BGP installation for learn, using gobgp.
It uses Linux-Network-Namespace for Network Dividing.

Following is Network topology.

```
+-----+ +-----+               +-----+ +-----+                +-----+ +-----+
| ns0 | | ns1 |               | ns2 | | ns3 |                | ns4 | | ns5 |
+-----+ +-----+               +-----+ +-----+                +-----+ +-----+
  <em0> <em1>                   <em2> <em3>                    <em4> <em5>
    .2  .3                        .2  .3                         .2  .3
     |   |                         |   |                          |   |
    +-----+                       +-----+                        +-----+
    | br0 |                       | br1 |                        | br2 |
    +-----+                       +-----+                        +-----+
       |                             |                              |
  10.0.10.0/24                  10.0.20.0/24                   10.0.30.0/24
       |                             |                              |
      .1                            .1                             .1
  +<bgp0eth0>+               +---<bgp1eth0>---+               +<bgp2eth0>+
  |          |               |                |               |          |
  |   BGP0   |               |      BGP1      |               |   BGP2   |
  |  AS0000  |               |     AS0000     |               |  AS0000  |
  |   <bgp0eth1>.1-------.2<bgp1eth1>  <bgp1eth2>.1-------.2<bgp2eth1>   |
  |          |               |                |               |          |
  |          |    192.168    |                |    192.168    |          |
  +----------+   .10.0/24    +----------------+   .20.0/24    +----------+
```


