set(component_srcs "adcservice.cpp")

idf_component_register(SRCS "adcservice.cpp" "${component_srcs}"
                       PRIV_REQUIRES driver esp_adc
                       INCLUDE_DIRS "include")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-missing-field-initializers")
