set(i2cbus_src I2CBus.cpp)
add_library(i2cbus SHARED ${i2cbus_src})
install(TARGETS i2cbus DESTINATION lib)

set(lsm303_src LSM303.cpp)
add_library(lsm303 SHARED ${lsm303_src})
target_link_libraries(lsm303 i2cbus)
install(TARGETS lsm303 DESTINATION lib)
