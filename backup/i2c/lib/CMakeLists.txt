set(lib_src i2cfunc.c)
add_library(i2cfunc SHARED ${lib_src})

install(TARGETS i2cfunc DESTINATION lib)
