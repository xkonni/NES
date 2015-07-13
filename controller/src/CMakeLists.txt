cmake_minimum_required(VERSION 2.8)

add_subdirectory(lib)

PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS messages.proto)

set(motor-daemon_src motor-daemon.cpp messages.pb.cc)
add_executable(motor-daemon ${motor-daemon_src})
target_link_libraries(motor-daemon
  ${CMAKE_SOURCE_DIR}/../../BBBIOlib/libBBBio.a
  protobuf
  pthread
)

set(sensor-daemon_src sensor-daemon.cpp)
add_executable(sensor-daemon ${sensor-daemon_src})
target_link_libraries(sensor-daemon
  lsm303
  i2cbus
)
