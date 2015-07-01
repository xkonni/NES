cmake_minimum_required(VERSION 2.8)

project(daemon)
add_definitions ("-Wall")

set(daemon_src daemon.c)
add_executable(daemon ${daemon_src})
install(TARGETS daemon DESTINATION .)

