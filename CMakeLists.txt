cmake_minimum_required(VERSION 3.0)
project(spurv)

if(WIN32)
  add_definitions(/std:c++latest /arch:AVX2)
else()
	add_definitions(-std=c++2a -fconcepts -mavx -fPIC)
endif()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/examples)

set(HCONLIB_ROOT ${CMAKE_HOME_DIRECTORY}/../HConLib)


set(SRC_DIR ${CMAKE_HOME_DIRECTORY}/src)

set(SRC_NAMES ${SRC_DIR}/constant_registry.cpp ${SRC_DIR}/utils.cpp
	${SRC_DIR}/uniforms.cpp ${SRC_DIR}/types.cpp
	${SRC_DIR}/event_registry.cpp ${SRC_DIR}/variable_registry.cpp
	${SRC_DIR}/pointers.cpp ${SRC_DIR}/control_flow.cpp)


include_directories(${HCONLIB_ROOT}/include)


add_library(spurv ${SRC_NAMES})
