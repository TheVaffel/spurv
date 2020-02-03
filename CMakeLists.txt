cmake_minimum_required(VERSION 3.0)
project(spurv)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/examples)

set(HCONLIB_ROOT /home/haakon/Documents/Code/C++/HConLib)

add_definitions(-std=c++1z -fconcepts)

set(SRC_DIR ${CMAKE_HOME_DIRECTORY}/src)

set(SRC_NAMES ${SRC_DIR}/constant_registry.cpp ${SRC_DIR}/utils.cpp ${SRC_DIR}/uniforms.cpp ${SRC_DIR}/types.cpp ${SRC_DIR}/variable_registry.cpp)


include_directories(${HCONLIB_ROOT}/include)

add_library(spurv ${SRC_NAMES})


