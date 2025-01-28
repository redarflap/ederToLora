set(component_srcs "onewireservice.cpp")

idf_component_register(SRCS "onewireds18b20.cpp" "onewiredevice.cpp" "onewireservice.cpp" "onewirebus.cpp" "${component_srcs}"
                       PRIV_REQUIRES driver
                       INCLUDE_DIRS "include")
                       set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-missing-field-initializers")
