set(component_srcs "uartservice.cpp")

idf_component_register(SRCS "uartservice.cpp" "${component_srcs}"
                       PRIV_REQUIRES driver
                       INCLUDE_DIRS "include")
                       set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-missing-field-initializers")
