
LIBSLANKDEV := $(HOME)/git/libslankdev
LIBDPDKCPP  := $(HOME)/git/libdpdk_cpp
include $(LIBDPDKCPP)/mk/dpdk.cpp.mk
include $(LIBSLANKDEV)/mk/rules.mk

CXXFLAGS += -std=c++11 -Wno-format-security
CXXFLAGS += -I$(LIBSLANKDEV) -I$(LIBDPDKCPP)
CXXFLAGS += $(DPDK_CXXFLAGS)
LDFLAGS  += $(DPDK_LDFLAGS)

SRC = main.cc
OBJ = $(SRC:.cc=.o)
TARGET = petorun

all: $(OBJ)
	@echo LD $(TARGET)
	@$(CXX) $(CXXFLAGS) $(OBJ) -o $(TARGET) $(LDFLAGS)

clean:
	rm -f $(OBJ) $(TARGET) *.o *.out

