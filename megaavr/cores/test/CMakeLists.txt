##########################################################################

cmake_minimum_required(VERSION 3.0)

##########################################################################

include_directories(include)
include_directories(external/libvireg/include)
include_directories(external/catch/v.2.7.0/include)

##########################################################################

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

##########################################################################

add_subdirectory(external/libvireg) # For building libvireg

##########################################################################

set(TEST_TARGET testArduinoCore-megaavr)

set(TEST_SRCS
  src/test_main.cpp
  ../arduino/NANO_Compat.cpp
)

##########################################################################

add_definitions(-DHOST_BUILD)
add_definitions(-DAVR_NANO_4809_328MODE)

##########################################################################

add_executable(${TEST_TARGET} ${TEST_SRCS})
target_link_libraries(${TEST_TARGET} vireg)

##########################################################################

add_custom_command(
  TARGET ${TEST_TARGET} POST_BUILD
  COMMENT "Executing unit tests"
  COMMAND "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TEST_TARGET}"
)

##########################################################################
