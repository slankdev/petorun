

GRPCXX := $(HOME)/git/grpcxx
GRPCXX_PROTO = gobgp
GRPCXX_PROTO_PATH = $(HOME)/git/petorun/gobgp/rib_manager
include $(GRPCXX)/mk/grpc.vars.mk
CXXFLAGS += $(GRPC_CXXFLAGS)
LDFLAGS  += $(GRPC_LDFLAGS)

TARGET = a.out
SRC = main.cc $(API).grpc.pb.cc $(API).pb.cc
OBJ = $(SRC:.cc=.o)

all: $(TARGET)

clean:
	rm -f $(TARGET) $(OBJ)

LIBSLANKDEV := $(HOME)/git/libslankdev
include $(LIBSLANKDEV)/mk/rules.mk
include $(GRPCXX)/mk/grpc.rules.mk

