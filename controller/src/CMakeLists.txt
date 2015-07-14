cmake_minimum_required(VERSION 2.8)
site_name(HOST)

add_subdirectory(lib)

PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS messages.proto)

set(controller-daemon_src controller-daemon.cpp messages.pb.cc)
add_executable(controller-daemon ${controller-daemon_src})
target_link_libraries(controller-daemon
  protobuf
  pthread
)

set(motor-daemon_src motor-daemon.cpp messages.pb.cc)
add_executable(motor-daemon ${motor-daemon_src})
if(${HOST} STREQUAL "beaglebone")
  target_link_libraries(motor-daemon
    ${CMAKE_SOURCE_DIR}/../../BBBIOlib/libBBBio.a
    protobuf
    pthread
  )
else()
  target_link_libraries(motor-daemon
    protobuf
    pthread
  )
endif()

set(sensor-daemon_src sensor-daemon.cpp)
add_executable(sensor-daemon ${sensor-daemon_src})
if(${HOST} STREQUAL "beaglebone")
  target_link_libraries(sensor-daemon
    lsm303
    i2cbus
    protobuf
    pthread
  )
else()
  target_link_libraries(sensor-daemon
    protobuf
    pthread
  )
endif()
