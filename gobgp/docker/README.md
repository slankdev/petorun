
# Leaning BGP with GoBGP and NETNS

This is BGP installation for learn, using gobgp.
It uses Linux-Network-Namespace for Network Dividing.

Following is Network topology.

```
@XXX: XXX is container name

   +--------+                +--------+
   | @user0 |                | @user1 |
   +--------+                +--------+
     <em0>                      <em1>
      .20                        .20
       |                          |
     unet0                      unet1
  10.0.10.0/24               10.0.20.0/24
       |                          |
      .10                        .10
  +<bgp0eth0>+               +<bgp1eth0>+
  |          |               |          |
  |  @BGP0   |               |  @BGP1   |
  | AS65000  |               | AS65001  |
  |   <bgp0eth1>.10-----.20<bgp1eth1>   |
  |          |               |          |
  |          |    192.168    |          |
  +----------+   .10.0/24    +----------+
	                  bnet
```



