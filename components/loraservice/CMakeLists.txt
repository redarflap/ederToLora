set(component_srcs "loraservice.cpp")

idf_component_register(SRCS "loraservice.cpp" "${component_srcs}"
                       PRIV_REQUIRES driver
                       INCLUDE_DIRS "include")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-missing-field-initializers")
