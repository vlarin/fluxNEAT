cmake_minimum_required(VERSION 3.15)
project(fluxNEAT)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMakeModules")

# Build all artifacts into single folder (thus make it all runnable)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set(CMAKE_PDB_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

# Check for Mac OS X
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(MACOSX TRUE)
endif()

# Disable "smart" linking of boost, we can figure all linkages by ourselves
add_definitions("-DBOOST_ALL_NO_LIB")

# Uncomment if it fails to find boost
#set(Boost_DEBUG ON)

set(Boost_USE_STATIC_LIBS ON)
find_package( Boost 1.68 REQUIRED COMPONENTS program_options serialization )

include_directories(ThirdParty)
include_directories(${Boost_INCLUDE_DIRS})

# Windows Proprietary Support
include(generate_product_version)

# Git Revision
execute_process(COMMAND git log --pretty=format:'%h' -n 1
        OUTPUT_VARIABLE GIT_REV
        ERROR_QUIET)

# Check whether we got any revision (which isn't
# always the case, e.g. when someone downloaded a zip
# file from Github instead of a checkout or just no git properly installed)
if ("${GIT_REV}" STREQUAL "")
    set(GIT_REV "0")
else()
    string(STRIP "${GIT_REV}" GIT_REV)
    string(SUBSTRING "${GIT_REV}" 1 7 GIT_REV)
endif()

# ML Sandbox
add_subdirectory(MLSandbox)

# fluxNEAT Common
add_subdirectory(Common)

# fluxNEAT Evolution Training
add_subdirectory(Training)

# fluxNEAT Cortex Azu
add_subdirectory(Cortex)

# fluxNEAT Console Sandbox
add_subdirectory(Sandbox)

# fluxNEAT Console Cortex Sandbox
add_subdirectory(CortexSandbox)