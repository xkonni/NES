cmake_minimum_required(VERSION 2.8)

project(daemon)
include_directories(${CMAKE_SOURCE_DIR}/../../BBBIOlib, ${CMAKE_SOURCE_DIR}/../../BBBIOlib/BBBio_lib)
add_definitions ("-Wall")

set(motor-daemon_src motor-daemon.cpp)
add_executable(motor-daemon ${motor-daemon_src})
target_link_libraries(motor-daemon ${CMAKE_SOURCE_DIR}/../../BBBIOlib/libBBBio.a)
install(TARGETS motor-daemon DESTINATION .)

