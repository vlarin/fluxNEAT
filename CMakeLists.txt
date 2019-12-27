cmake_minimum_required(VERSION 3.15)
project(fluxNEAT)

set(CMAKE_CXX_STANDARD 11)

# Disable "smart" linking of boost, we can figure all linkages by ourselves
add_definitions("-DBOOST_ALL_NO_LIB")

# Uncomment if it fails to find boost
#set(Boost_DEBUG ON)

set(Boost_USE_STATIC_LIBS ON)
find_package( Boost 1.68 REQUIRED COMPONENTS program_options serialization )

include_directories(ThirdParty)
include_directories(${Boost_INCLUDE_DIRS})

# ML Sandbox
include(MLSandbox/CMakeLists.txt)

# fluxNEAT Common
include(Common/CMakeLists.txt)