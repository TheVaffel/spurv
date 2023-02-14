cmake_minimum_required(VERSION 3.0)
project(spurv)

if(WIN32)
  add_definitions(/std:c++latest /arch:AVX2)
else()
  add_definitions(-std=c++2a -mavx -fPIC)
endif()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/examples)

set(HCONLIB_ROOT ${CMAKE_HOME_DIRECTORY}/../HConLib)
set(HCONLIB_INCLUDE_DIR ${HCONLIB_ROOT}/include)

set(FLAWED_PATH ${CMAKE_HOME_DIRECTORY}/../flawed)
set(FLAWED_INCLUDE_DIR ${FLAWED_PATH}/include)

set(SRC_DIR ${CMAKE_HOME_DIRECTORY}/src)

set(SRC_NAMES ${SRC_DIR}/constant_registry.cpp ${SRC_DIR}/utils.cpp
  ${SRC_DIR}/uniforms.cpp ${SRC_DIR}/types.cpp
  ${SRC_DIR}/event_registry.cpp ${SRC_DIR}/variable_registry.cpp
  ${SRC_DIR}/pointers.cpp ${SRC_DIR}/control_flow.cpp)

include_directories(${HCONLIB_INCLUDE_DIR}
  ${FLAWED_INCLUDE_DIR})

add_library(spurv ${SRC_NAMES})

set(FLAWED_OUTPUT_PATH flawed_tests)
set(TEST_DIR tests)

include(${FLAWED_PATH}/FlawedTest.cmake)

flawed_add_tests(${TEST_DIR}/fragment_tests.cpp)

flawed_write_test_target(OpenImageIO HGraf)
