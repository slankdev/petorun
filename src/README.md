
# Router

```
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
$ make
$ sudo ./petorun -f petorun.conf
```


