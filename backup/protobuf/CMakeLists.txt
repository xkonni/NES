cmake_minimum_required(VERSION 2.8)
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set("CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE_RELEASE}")
  message("setting CMAKE_BUILD_TYPE_RELEASE")
else()
  message("CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE}")
endif()

project(BBB_PROTOBUF)
find_package(Protobuf REQUIRED)
PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS sensor.proto)
# HACK: add command to compile protobuf for python
add_custom_command(
  OUTPUT sensor_pb2.py
  COMMAND protoc
  ARGS -I ${CMAKE_CURRENT_SOURCE_DIR} --python_out ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/sensor.proto
)
# HACK: add it as a dependency to the cpp code
set_property(SOURCE test.cpp APPEND PROPERTY OBJECT_DEPENDS sensor_pb2.py)
set(test_src test.cpp sensor.pb.cc)
add_executable(test ${test_src})
target_link_libraries(test protobuf pthread)

