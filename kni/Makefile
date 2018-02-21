

LIBSLANKDEV := $(HOME)/git/libslankdev
LIBDPDKCPP  := $(HOME)/git/libdpdk_cpp
include $(LIBDPDKCPP)/mk/dpdk.cpp.mk

CXXFLAGS += -std=c++11 -Wno-format-security
CXXFLAGS += -I$(LIBDPDKCPP) $(DPDK_CXXFLAGS)
CXXFLAGS += -I$(LIBSLANKDEV)
LDFLAGS  += $(DPDK_LDFLAGS)

all:
	$(CXX) $(CXXFLAGS) main.cc $(LDFLAGS)


