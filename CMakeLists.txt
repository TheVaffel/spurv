cmake_minimum_required(VERSION 3.0)
project(spurv)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/examples)

set(HCONLIB_ROOT /home/haakon/Documents/Code/C++/HConLib)

add_definitions(-std=c++1z)

set(SRC_DIR ${CMAKE_HOME_DIRECTORY}/src)

set(SRC_NAMES ${SRC_DIR}/constant_registry.cpp ${SRC_DIR}/utils.cpp ${SRC_DIR}/uniforms.cpp ${SRC_DIR}/types.cpp)

set(HDR_NAMES expressions_impl types uniforms_impl values constant_registry shaders types_impl values_impl constant_registry_impl shaders_impl utils declarations uniforms utils_impl)

include_directories(${HCONLIB_ROOT}/include)

add_library(spurv ${SRC_NAMES})


