
LIBSLANKDEV := $(HOME)/git/libslankdev
CXXFLAGS += -I$(LIBSLANKDEV) -std=c++11

API = gobgp
GRPCOBJ = $(API).grpc.pb.o $(API).pb.o
GRPC_CXXFLAGS = -std=c++11 `pkg-config --cflags protobuf grpc++ grpc`
GRPC_LDFLAGS  = `pkg-config --libs protobuf grpc++ grpc` \
								-Wl,--no-as-needed -lgrpc++_reflection -Wl,--as-needed -ldl

CXXFLAGS += $(GRPC_CXXFLAGS)
LDFLAGS  += $(GRPC_LDFLAGS)

TARGET = a.out
SRC = main.cc \
			gobgp.grpc.pb.cc gobgp.pb.cc
OBJ = $(SRC:.cc=.o)

all: $(TARGET)

grpcbuild:
	protoc -I. --cpp_out=. $(API).proto
	protoc -I. --grpc_out=. --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` $(API).proto
	$(CXX) $(GRPC_CXXFLAGS) -c -o $(API).grpc.pb.o $(API).grpc.pb.cc
	$(CXX) $(GRPC_CXXFLAGS) -c -o $(API).pb.o $(API).pb.cc

grpcclean:
	rm -f \
		$(API).grpc.pb.cc \
		$(API).grpc.pb.h  \
		$(API).grpc.pb.o  \
		$(API).pb.cc  \
		$(API).pb.h   \
		$(API).pb.o

clean:
	rm -f $(OBJ) *.out *.o

include $(LIBSLANKDEV)/mk/rules.mk

