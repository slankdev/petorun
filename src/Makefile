
LIBSLANKDEV := $(HOME)/git/libslankdev
LIBDPDKCPP  := $(HOME)/git/libdpdk_cpp
include $(LIBDPDKCPP)/mk/dpdk.cpp.mk

CXXFLAGS += -std=c++11 -Wno-format-security
CXXFLAGS += -I$(LIBSLANKDEV) -I$(LIBDPDKCPP)
CXXFLAGS += $(DPDK_CXXFLAGS)
LDFLAGS  += $(DPDK_LDFLAGS)

SRC = main.cc petorun_netdev.cc
OBJ = $(SRC:.cc=.o)
TARGET = petorun

all: $(TARGET)

$(TARGET): $(OBJ)

clean:
	rm -f $(OBJ) $(TARGET) *.o *.out

run:
	sudo ./petorun \
		-w 0000:00:00.0 \
		--vdev=net_af_packet0,iface=veth0 \
		--vdev=net_af_packet1,iface=veth1

include $(LIBSLANKDEV)/mk/rules.mk

