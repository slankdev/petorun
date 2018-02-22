
LIBSLANKDEV := $(HOME)/git/libslankdev
CXXFLAGS += -I$(LIBSLANKDEV) -std=c++11

SRC = main.cc
OBJ = $(SRC:.cc=.o)
TARGET = a.out

all: $(TARGET)

clean:
	rm -f *.o *.out

include $(LIBSLANKDEV)/mk/rules.mk
