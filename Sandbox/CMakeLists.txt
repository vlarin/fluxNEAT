# fluxNEAT Console Sandbox
generate_product_version(fluxNeat-Sandbox-ProductVersionFiles
        NAME fluxNEAT.Sandbox
        VERSION_MAJOR 0
        VERSION_MINOR 1
        VERSION_PATCH 0
        VERSION_REVISION 0
        COMPANY_COPYRIGHT "Temporal Games Inc. (c) Copyright 2019-2020. All rights reserved."
        COMPANY_NAME "Temporal Games Inc.")

add_executable(fluxNEAT.Sandbox main.cpp ${fluxNeat-Sandbox-ProductVersionFiles})

target_link_libraries(fluxNEAT.Sandbox ${Boost_LIBRARIES})
target_link_libraries(fluxNEAT.Sandbox fluxNEAT.Common)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_link_libraries(fluxNEAT.Sandbox ${CMAKE_SOURCE_DIR}/ThirdParty/libs/mlpack-gd.lib)
else()
    target_link_libraries(fluxNEAT.Sandbox ${CMAKE_SOURCE_DIR}/ThirdParty/libs/mlpack.lib)
endif()

# Uncomment for linker debug
#target_link_options(fluxNEAT.Sandbox PUBLIC LINKER:/VERBOSE)