cmake_minimum_required(VERSION 2.8)
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set("CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE_RELEASE}")
  message("setting CMAKE_BUILD_TYPE_RELEASE")
else()
  message("CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE}")
endif()

project(BBB_I2C)
# set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")

add_subdirectory(lib)
include_directories(${CMAKE_SOURCE_DIR}/lib)

set(test_src test.cpp)
add_executable(test ${test_src})
target_link_libraries(test i2cbus lsm303)

