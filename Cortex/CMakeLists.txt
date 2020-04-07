add_definitions("-DFLUX_EXPORT")

generate_product_version(fluxNeat-Cortex-ProductVersionFiles
        NAME fluxNEAT.Cortex
        VERSION_MAJOR 0
        VERSION_MINOR 1
        VERSION_PATCH 0
        VERSION_REVISION 0
        FILE_DESCRIPTION "fluxNEAT AI Cortex Azu Library ${GIT_REV}"
        COMPANY_COPYRIGHT "Temporal Games Inc. (c) Copyright 2019-2020. All rights reserved."
        COMPANY_NAME "Temporal Games Inc.")

add_library(fluxNEAT.Cortex
        src/cortex_black_box.cpp
        src/cortex_transition.cpp
        src/cortex_targeted_transition.cpp
        src/cortex_column.cpp
        src/cortex_layer.cpp
        src/cortex_decision_layer.cpp

        #src/include/neat_activity_unit_impl.h

        ${fluxNeat-Cortex-ProductVersionFiles}

        include/flux/cortex/cortex_column.h
        include/flux/cortex/cortex_black_box.h
        include/flux/cortex/cortex_transition.h
        include/flux/cortex/cortex_targeted_transition.h
        include/flux/cortex/cortex_layer.h
        include/flux/cortex/cortex_decision_layer.h
        include/flux/cortex.h)

target_link_libraries(fluxNEAT.Cortex ${Boost_LIBRARIES})
target_include_directories(fluxNEAT.Cortex PUBLIC ${CMAKE_SOURCE_DIR}/Common/include)
target_include_directories(fluxNEAT.Cortex PUBLIC ${CMAKE_SOURCE_DIR}/Cortex/include)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    if (MACOSX)
        target_link_libraries(fluxNEAT.Cortex ${CMAKE_SOURCE_DIR}/ThirdParty/libs/libmlpack-gd.a)
    else()
        target_link_libraries(fluxNEAT.Cortex ${CMAKE_SOURCE_DIR}/ThirdParty/libs/mlpack-gd.lib)
    endif()
else()
    if (MACOSX)
        target_link_libraries(fluxNEAT.Cortex ${CMAKE_SOURCE_DIR}/ThirdParty/libs/libmlpack.a)
    else()
        target_link_libraries(fluxNEAT.Cortex ${CMAKE_SOURCE_DIR}/ThirdParty/libs/mlpack.lib)
    endif()
endif()

target_link_libraries(fluxNEAT.Cortex fluxNEAT.Common)

#target_link_options(fluxNEAT.Cortex PUBLIC LINKER:/FORCE)