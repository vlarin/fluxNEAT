add_definitions("-DFLUX_EXPORT")

add_library(fluxNEAT.Common SHARED
        include/flux/common.h
        src/neural_input.cpp
        src/neural_output.cpp

        include/flux/raw_sensor_unit.h
        include/flux/augmented_sensor_unit.h
        include/flux/activity_unit.h
        include/flux/feedback/feedback_unit.h)

target_link_libraries(fluxNEAT.Common ${Boost_LIBRARIES})
target_include_directories(fluxNEAT.Common PUBLIC ${CMAKE_SOURCE_DIR}/Common/include)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_link_libraries(fluxNEAT.Common ${CMAKE_SOURCE_DIR}/ThirdParty/libs/mlpack-gd.lib)
else()
    target_link_libraries(fluxNEAT.Common ${CMAKE_SOURCE_DIR}/ThirdParty/libs/mlpack.lib)
endif()