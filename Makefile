
HCONLIB_ROOT = /home/haakon/Documents/Code/C++/HConLib

CFLAGS=-Wall -std=c++1z -g

LIBS=-lWingine -lvulkan -lFlatAlg  -l Winval -l X11
LIB_DIRS=-L$(HCONLIB_ROOT)/lib -L$(VULKAN_SDK)/lib
INCLUDE_DIRS=-I$(HCONLIB_ROOT)/include -I.

ANTI_WARNINGS=-Wno-delete-non-virtual-dtor

all: test test2

test: test.cpp spurv.hpp
	g++ $< -o $@ $(INCLUDE_DIRS) $(LIB_DIRS) $(LIBS) $(CFLAGS) $(ANTI_WARNINGS)

test2:	test2.cpp spurv.hpp
	g++ $< -o $@ $(INCLUDE_DIRS) $(LIB_DIRS) $(LIBS) $(CFLAGS) $(ANTI_WARNINGS)

clean:  
	rm test2 test
