cmake_minimum_required(VERSION 2.8)
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set("CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE_RELEASE}")
  message("setting CMAKE_BUILD_TYPE_RELEASE")
else()
  message("CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE}")
endif()

project(BBB_I2C)
# set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})

add_subdirectory(lib)
include_directories(${CMAKE_SOURCE_DIR}/lib)

set(test_src test.c)
add_executable(test ${test_src})
target_link_libraries(test i2cfunc)

set(lsm303_src lsm303.c)
add_executable(lsm303 ${lsm303_src})
target_link_libraries(lsm303 i2cfunc)
