cmake_minimum_required(VERSION 2.8)

site_name(HOST)
if(${HOST} STREQUAL "beaglebone")
  message("building on beaglebone")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DBBB_HOST")
else()
  message("not building on beaglebone")
endif()

if($ENV{CAN})
  message("using CAN bus")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DBBB_CAN")
else()
  message("using tcp sockets")
endif()

project(controller)
find_package(Protobuf REQUIRED)
add_definitions ("-Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")

if(${HOST} STREQUAL "beaglebone")
  include_directories(
    ${CMAKE_SOURCE_DIR}/src/lib/BBBIOlib
    ${CMAKE_SOURCE_DIR}/src/lib/BBBIOlib/BBBio_lib
    ${CMAKE_SOURCE_DIR}/src/lib
  )
else()
  include_directories(
    ${CMAKE_SOURCE_DIR}/src/lib
  )
endif()
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})

add_subdirectory(src)
