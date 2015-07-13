cmake_minimum_required(VERSION 2.8)

add_subdirectory(lib)

set(motor-daemon_src motor-daemon.cpp)
add_executable(motor-daemon ${motor-daemon_src})
target_link_libraries(motor-daemon ${CMAKE_SOURCE_DIR}/../../BBBIOlib/libBBBio.a)

set(sensor-daemon_src sensor-daemon.cpp)
add_executable(sensor-daemon ${sensor-daemon_src})
target_link_libraries(sensor-daemon lsm303 i2cbus)
