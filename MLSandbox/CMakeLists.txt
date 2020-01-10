# NEAT
add_executable(ML-NEAT-Sandbox neat.cpp)

target_link_libraries(ML-NEAT-Sandbox ${Boost_LIBRARIES})

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_link_libraries(ML-NEAT-Sandbox ${CMAKE_SOURCE_DIR}/ThirdParty/libs/mlpack-gd.lib)
else()
    target_link_libraries(ML-NEAT-Sandbox ${CMAKE_SOURCE_DIR}/ThirdParty/libs/mlpack.lib)
endif()

# Uncomment for linker debug
#target_link_options(ML-NEAT-Sandbox PUBLIC LINKER:/VERBOSE)

# BASIC
add_executable(ML-RAW-Sandbox main.cpp)

target_link_libraries(ML-RAW-Sandbox ${Boost_LIBRARIES})

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_link_libraries(ML-RAW-Sandbox ${CMAKE_SOURCE_DIR}/ThirdParty/libs/mlpack-gd.lib)
else()
    target_link_libraries(ML-RAW-Sandbox ${CMAKE_SOURCE_DIR}/ThirdParty/libs/mlpack.lib)
endif()

# Uncomment for linker debug
#target_link_options(ML-RAW-Sandbox PUBLIC LINKER:/VERBOSE)