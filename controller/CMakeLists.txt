cmake_minimum_required(VERSION 2.8)

project(controller)
add_definitions ("-Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
include_directories(
  ${CMAKE_SOURCE_DIR}/src/lib
  ${CMAKE_SOURCE_DIR}/../../BBBIOlib
  ${CMAKE_SOURCE_DIR}/../../BBBIOlib/BBBio_lib
)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})
find_package(Protobuf REQUIRED)

add_subdirectory(src)
