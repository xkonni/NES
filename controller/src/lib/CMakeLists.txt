PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS messages.proto)

set(i2cbus_src I2CBus.cpp)
add_library(i2cbus SHARED ${i2cbus_src})
install(TARGETS i2cbus DESTINATION lib)

set(lsm303_src LSM303.cpp)
add_library(lsm303 SHARED ${lsm303_src})
target_link_libraries(lsm303 i2cbus)
install(TARGETS lsm303 DESTINATION lib)

set(nes_src nes.cpp)
add_library(nes SHARED ${nes_src})
target_link_libraries(nes protobuf pthread)
install(TARGETS nes DESTINATION lib)

set(nes-socket_src nes-socket.cpp messages.pb.cc)
add_library(nes-socket SHARED ${nes-socket_src})
target_link_libraries(nes-socket nes protobuf pthread)
install(TARGETS nes-socket DESTINATION lib)

