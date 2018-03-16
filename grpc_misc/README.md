
# Instration of gRPC

install shell script

```
#!/bin/sh

sudo apt install \
 build-essential autoconf libtool pkg-config \
 libgflags-dev libgtest-dev clang libc++-dev

git clone https://github.com/grpc/grpc && cd grpc
git submodule update --init
make && sudo make install
```

template makefile

```

```

