cmake_minimum_required(VERSION 2.8)

project(daemon)
include_directories(${CMAKE_SOURCE_DIR}/../../BBBIOlib, ${CMAKE_SOURCE_DIR}/../../BBBIOlib/BBBio_lib)
add_definitions ("-Wall")

set(daemon_src daemon.c)
add_executable(daemon ${daemon_src})
target_link_libraries(daemon ${CMAKE_SOURCE_DIR}/../../BBBIOlib/libBBBio.a)
install(TARGETS daemon DESTINATION .)

