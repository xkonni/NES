cmake_minimum_required(VERSION 2.8)
site_name(HOST)

add_subdirectory(lib)

set(controller-daemon_src controller-daemon.cpp)
add_executable(controller-daemon ${controller-daemon_src})
target_link_libraries(controller-daemon
  nes
  nes-socket
)

set(motor-daemon_src motor-daemon.cpp)
add_executable(motor-daemon ${motor-daemon_src})
if(${HOST} STREQUAL "beaglebone")
  target_link_libraries(motor-daemon
    ${CMAKE_SOURCE_DIR}/src/lib/BBBIOlib/libBBBio.a
    nes
    nes-socket
  )
else()
  target_link_libraries(motor-daemon
    nes
    nes-socket
  )
endif()

set(sensor-daemon_src sensor-daemon.cpp)
add_executable(sensor-daemon ${sensor-daemon_src})
if(${HOST} STREQUAL "beaglebone")
  target_link_libraries(sensor-daemon
    lsm303
    i2cbus
    nes
    nes-socket
  )
else()
  target_link_libraries(sensor-daemon
    nes
    nes-socket
  )
endif()
