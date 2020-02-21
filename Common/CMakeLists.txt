add_definitions("-DFLUX_EXPORT")

generate_product_version(fluxNeat-Common-ProductVersionFiles
        NAME fluxNEAT.Common
        VERSION_MAJOR 0
        VERSION_MINOR 1
        VERSION_PATCH 0
        VERSION_REVISION ${GIT_REV}
        FILE_DESCRIPTION "fluxNEAT AI Common Library"
        COMPANY_COPYRIGHT "Temporal Games Inc. (c) Copyright 2019-2020. All rights reserved."
        COMPANY_NAME "Temporal Games Inc.")

add_library(fluxNEAT.Common
        include/flux/common.h
        src/neural_input.cpp
        src/neural_output.cpp
        src/context_unit.cpp
        src/manual_raw_input.cpp
        src/single_activity_black_box.cpp
        src/neat_activity_unit.cpp
        src/neat_activity_unit_impl.cpp
        src/neat_entity_descriptor.cpp
        src/global_error.cpp

        src/include/neat_activity_unit_impl.h

        ${fluxNeat-Common-ProductVersionFiles}

        include/flux/black_box.h
        include/flux/context_unit.h
        include/flux/output_unit.h
        include/flux/raw_sensor_unit.h
        include/flux/augmented_sensor_unit.h
        include/flux/activity_unit.h
        include/flux/feedback/feedback_unit.h
        include/flux/manual/unit_context.h
        include/flux/manual/manual_raw_input_unit.h
        include/flux/manual/single_activity_black_box.h
        include/flux/neat/neat_activity_unit.h
        include/flux/neat/neat_entity_descriptor.h
        include/flux/context_registry.h
        include/flux/global_error.h)

target_link_libraries(fluxNEAT.Common ${Boost_LIBRARIES})
target_include_directories(fluxNEAT.Common PUBLIC ${CMAKE_SOURCE_DIR}/Common/include)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    if (MACOSX)
        target_link_libraries(fluxNEAT.Common ${CMAKE_SOURCE_DIR}/ThirdParty/libs/libmlpack-gd.a)
    else()
        target_link_libraries(fluxNEAT.Common ${CMAKE_SOURCE_DIR}/ThirdParty/libs/mlpack-gd.lib)
    endif()
else()
    if (MACOSX)
        target_link_libraries(fluxNEAT.Common ${CMAKE_SOURCE_DIR}/ThirdParty/libs/libmlpack.a)
    else()
        target_link_libraries(fluxNEAT.Common ${CMAKE_SOURCE_DIR}/ThirdParty/libs/mlpack.lib)
    endif()
endif()

#target_link_options(fluxNEAT.Common PUBLIC LINKER:/FORCE)