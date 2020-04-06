# fluxNEAT Console Cortex Sandbox
generate_product_version(fluxNeat-CortexSandbox-ProductVersionFiles
        NAME fluxNEAT.Sandbox
        VERSION_MAJOR 0
        VERSION_MINOR 1
        VERSION_PATCH 0
        VERSION_REVISION ${GIT_REV}
        FILE_DESCRIPTION "fluxNEAT AI Cortex Azu Sandbox"
        COMPANY_COPYRIGHT "Temporal Games Inc. (c) Copyright 2019-2020. All rights reserved."
        COMPANY_NAME "Temporal Games Inc.")

add_executable(fluxNEAT.CortexSandbox main.cpp ${fluxNeat-CortexSandbox-ProductVersionFiles})

target_link_libraries(fluxNEAT.CortexSandbox ${Boost_LIBRARIES})
target_link_libraries(fluxNEAT.CortexSandbox fluxNEAT.Common)
target_link_libraries(fluxNEAT.CortexSandbox fluxNEAT.Cortex)
target_link_libraries(fluxNEAT.CortexSandbox fluxNEAT.Training)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    if (MACOSX)
        target_link_libraries(fluxNEAT.CortexSandbox ${CMAKE_SOURCE_DIR}/ThirdParty/libs/libmlpack-gd.a)
    else()
        target_link_libraries(fluxNEAT.CortexSandbox ${CMAKE_SOURCE_DIR}/ThirdParty/libs/mlpack-gd.lib)
    endif()
else()
    if (MACOSX)
        target_link_libraries(fluxNEAT.CortexSandbox ${CMAKE_SOURCE_DIR}/ThirdParty/libs/libmlpack.a)
    else()
        target_link_libraries(fluxNEAT.CortexSandbox ${CMAKE_SOURCE_DIR}/ThirdParty/libs/mlpack.lib)
    endif()
endif()

# Uncomment for linker debug
#target_link_options(fluxNEAT.CortexSandbox PUBLIC LINKER:/VERBOSE)