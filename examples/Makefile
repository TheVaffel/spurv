
HCONLIB_ROOT = /home/haakon/Documents/Code/C++/HConLib
WINGINE_ROOT = /home/haakon/Documents/Code/C++/Wingine
SROOT = ..

CFLAGS=-Wall -std=c++1z -fconcepts -g -mavx

LIBS=-lWingine -lvulkan -lFlatAlg -lWinval -lX11 -ldl -lspurv
LIB_DIRS=-L$(HCONLIB_ROOT)/lib -L$(WINGINE_ROOT)/build -L../lib
INCLUDE_DIRS=-I$(HCONLIB_ROOT)/include -I. -I$(WINGINE_ROOT)/include

ANTI_WARNINGS=-Wno-delete-non-virtual-dtor

HDRS=$(SROOT)/src/declarations.hpp \
    $(SROOT)/src/types.hpp \
    $(SROOT)/src/values.hpp \
    $(SROOT)/src/shaders.hpp \
    $(SROOT)/include/spurv.hpp \
    $(SROOT)/src/uniforms.hpp \
    $(SROOT)/src/constant_registry.hpp \
    $(SROOT)/src/variable_registry.hpp

IMPL_HDRS= $(SROOT)/src/utils_impl.hpp \
    $(SROOT)/src/expressions_impl.hpp \
    $(SROOT)/src/uniforms_impl.hpp \
    $(SROOT)/src/types_impl.hpp \
    $(SROOT)/src/values_impl.hpp \
    $(SROOT)/src/shaders_impl.hpp \
    $(SROOT)/src/constant_registry_impl.hpp

all: test test2 texture_test

test: test.cpp $(HDRS) $(IMPL_HDRS)
	g++ $< -o $@ $(INCLUDE_DIRS) $(LIB_DIRS) $(LIBS) $(CFLAGS) $(ANTI_WARNINGS)

test2:	test2.cpp $(HDRS) $(IMPL_HDRS)
	g++ $< -o $@ $(INCLUDE_DIRS) $(LIB_DIRS) $(LIBS) $(CFLAGS) $(ANTI_WARNINGS)

texture_test:	texture_test.cpp $(HDRS) $(IMPL_HDRS)
	g++ $< -o $@ $(INCLUDE_DIRS) $(LIB_DIRS) $(LIBS) $(CFLAGS) $(ANTI_WARNINGS)

clean:  
	rm texture_test test2 test
