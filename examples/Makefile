
HCONLIB_ROOT = /home/haakon/Documents/Code/C++/HConLib
SROOT = ..

CFLAGS=-Wall -std=c++1z -g

LIBS=-lWingine -lvulkan -lFlatAlg  -l Winval -l X11
LIB_DIRS=-L$(HCONLIB_ROOT)/lib -L$(VULKAN_SDK)/lib
INCLUDE_DIRS=-I$(HCONLIB_ROOT)/include -I.

ANTI_WARNINGS=-Wno-delete-non-virtual-dtor

HDRS=$(SROOT)/src/declarations.hpp \
    $(SROOT)/src/types.hpp \
    $(SROOT)/src/values.hpp \
    $(SROOT)/src/shaders.hpp \
    $(SROOT)/include/spurv.hpp \
    $(SROOT)/src/uniforms.hpp \
    $(SROOT)/src/constant_registry.hpp

IMPL_HDRS= $(SROOT)/src/utils_impl.hpp \
    $(SROOT)/src/expressions_impl.hpp \
    $(SROOT)/src/uniforms_impl.hpp \
    $(SROOT)/src/types_impl.hpp \
    $(SROOT)/src/values_impl.hpp \
    $(SROOT)/src/shaders_impl.hpp

SRCS=$(SROOT)/src/types.cpp \
    $(SROOT)/src/uniforms.cpp \
    $(SROOT)/src/constant_registry.cpp \
    $(SROOT)/src/utils.cpp

all: test test2 texture_test

test: test.cpp $(SRCS) $(HDRS) $(IMPL_HDRS)
	g++ $< $(SRCS) -o $@ $(INCLUDE_DIRS) $(LIB_DIRS) $(LIBS) $(CFLAGS) $(ANTI_WARNINGS)

test2:	test2.cpp $(SRCS) $(HDRS) $(IMPL_HDRS)
	g++ $< $(SRCS) -o $@ $(INCLUDE_DIRS) $(LIB_DIRS) $(LIBS) $(CFLAGS) $(ANTI_WARNINGS)

texture_test:	texture_test.cpp $(SRCS) $(HDRS) $(IMPL_HDRS)
	g++ $< $(SRCS) -o $@ $(INCLUDE_DIRS) $(LIB_DIRS) $(LIBS) $(CFLAGS) $(ANTI_WARNINGS)

clean:  
	rm texture_test test2 test
