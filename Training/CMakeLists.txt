add_definitions("-DFLUX_EXPORT")

generate_product_version(fluxNeat-Training-ProductVersionFiles
        NAME fluxNEAT.Training
        VERSION_MAJOR 0
        VERSION_MINOR 2
        VERSION_PATCH 0
        VERSION_REVISION 0
        FILE_DESCRIPTION "fluxNEAT AI Evolution Training Library ${GIT_REV}"
        COMPANY_COPYRIGHT "Temporal Games Inc. (c) Copyright 2019-2020. All rights reserved."
        COMPANY_NAME "Temporal Games Inc.")

add_library(fluxNEAT.Training
        include/flux/training.h
        include/flux/training/neat_evolution_params.h
        include/flux/training/neat_activity_trainer.h
        include/flux/training/evaluation_output_unit.h

        ${fluxNeat-Training-ProductVersionFiles}

        src/include/neat_activity_trainer_impl.h
        src/include/hyper_neat_activity_trainer_impl.h

        src/neat_evolution_params.cpp
        src/neat_activity_trainer.cpp
        src/neat_activity_trainer_impl.cpp
        src/hyper_neat_activity_trainer_impl.cpp)

target_link_libraries(fluxNEAT.Training ${Boost_LIBRARIES})
target_include_directories(fluxNEAT.Training PUBLIC ${CMAKE_SOURCE_DIR}/Common/include)
target_include_directories(fluxNEAT.Training PUBLIC ${CMAKE_SOURCE_DIR}/Training/include)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    if (MACOSX)
        target_link_libraries(fluxNEAT.Training ${CMAKE_SOURCE_DIR}/ThirdParty/libs/libmlpack-gd.a)
    else()
        target_link_libraries(fluxNEAT.Training ${CMAKE_SOURCE_DIR}/ThirdParty/libs/mlpack-gd.lib)
    endif()
else()
    if (MACOSX)
        target_link_libraries(fluxNEAT.Training ${CMAKE_SOURCE_DIR}/ThirdParty/libs/libmlpack.a)
    else()
        target_link_libraries(fluxNEAT.Training ${CMAKE_SOURCE_DIR}/ThirdParty/libs/mlpack.lib)
    endif()
endif()

target_link_libraries(fluxNEAT.Training fluxNEAT.Common)

#target_link_options(fluxNEAT.Training PUBLIC LINKER:/FORCE)