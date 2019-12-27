add_definitions("-DFLUX_EXPORT")

add_library(fluxNEAT.Common SHARED
        Common/include/flux/common.h
        Common/src/neural_input.cpp
        Common/src/neural_output.cpp
        Common/include/flux/augmented_sensor_unit.h
        Common/include/flux/activity_unit.h)

target_link_libraries(fluxNEAT.Common ${Boost_LIBRARIES})

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_link_libraries(fluxNEAT.Common ${CMAKE_SOURCE_DIR}/ThirdParty/libs/mlpack-gd.lib)
else()
    target_link_libraries(fluxNEAT.Common ${CMAKE_SOURCE_DIR}/ThirdParty/libs/mlpack.lib)
endif()