PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS messages.proto)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

if(${HOST} STREQUAL "beaglebone")
  set(i2cbus_src I2CBus.cpp)
  add_library(i2cbus SHARED ${i2cbus_src})
  install(TARGETS i2cbus DESTINATION lib)

  set(lsm303_src LSM303.cpp)
  add_library(lsm303 SHARED ${lsm303_src})
  target_link_libraries(lsm303 i2cbus)
  install(TARGETS lsm303 DESTINATION lib)
endif()

set(nes_src nes.cpp)
add_library(nes SHARED ${nes_src} ${PROTO_SRCS} ${PROTO_HDRS})
target_link_libraries(nes ${PROTOBUF_LIBRARIES} pthread)
install(TARGETS nes DESTINATION lib)

set(nes-socket_src nes-socket.cpp)
add_library(nes-socket SHARED ${nes-socket_src} ${PROTO_SRCS} ${PROTO_HDRS})
target_link_libraries(nes-socket nes ${PROTOBUF_LIBRARIES} pthread)
install(TARGETS nes-socket DESTINATION lib)

set(nes-can_src nes-can.cpp)
add_library(nes-can SHARED ${nes-can_src} ${PROTO_SRCS} ${PROTO_HDRS})
target_link_libraries(nes-can ${PROTOBUF_LIBRARIES} nes)
install(TARGETS nes-can DESTINATION lib)
