
HCONLIB_ROOT = /home/haakon/Documents/Code/C++/HConLib

CFLAGS=-Wall -std=c++1z -g

LIBS=-lWingine -lvulkan -lFlatAlg  -l Winval -l X11
LIB_DIRS=-L$(HCONLIB_ROOT)/lib -L$(VULKAN_SDK)/lib
INCLUDE_DIRS=-I$(HCONLIB_ROOT)/include -I.

ANTI_WARNINGS=-Wno-delete-non-virtual-dtor

HDRS=declarations.hpp types.hpp nodes.hpp shaders.hpp spurv.hpp uniforms.hpp constant_registry.hpp
IMPL_HDRS= utils_impl.hpp expressions_impl.hpp uniforms_impl.hpp types_impl.hpp nodes_impl.hpp shaders_impl.hpp

SRCS=types.cpp uniforms.cpp constant_registry.cpp utils.cpp

all: test test2 texture_test

test: test.cpp $(SRCS) $(HDRS) $(IMPL_HDRS)
	g++ $< $(SRCS) -o $@ $(INCLUDE_DIRS) $(LIB_DIRS) $(LIBS) $(CFLAGS) $(ANTI_WARNINGS)

test2:	test2.cpp $(SRCS) $(HDRS) $(IMPL_HDRS)
	g++ $< $(SRCS) -o $@ $(INCLUDE_DIRS) $(LIB_DIRS) $(LIBS) $(CFLAGS) $(ANTI_WARNINGS)

texture_test:	texture_test.cpp $(SRCS) $(HDRS) $(IMPL_HDRS)
	g++ $< $(SRCS) -o $@ $(INCLUDE_DIRS) $(LIB_DIRS) $(LIBS) $(CFLAGS) $(ANTI_WARNINGS)

clean:  
	rm texture_test test2 test
