cmake_minimum_required(VERSION 3.0)

project(ProyectoFrontOffice)

# 1. Define some well-known folders
#- Place for compile binaries
set(CMAKE_BINARY_DIR ${PROJECT_SOURCE_DIR}/build)
# - Root for include files
set(INCLUDE_HOME ${PROJECT_SOURCE_DIR}/src)
# - In-house cmake  functions
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake-lib)

# 2. Find Boost without fixed routes
find_package(Boost REQUIRED)

if(Boost_FOUND)
    message("--> Boost encontrado en: ${Boost_INCLUDE_DIRS}")
    include_directories(${Boost_INCLUDE_DIRS})
else()
    message(FATAL_ERROR "Boost no encontrado en el sistema. Instálalo y verifica las rutas.")
endif()

# 3. Print some stuff
message("--> CMake extra functions: "${CMAKE_MODULE_PATH})
message("--> Binaries: " ${CMAKE_BINARY_DIR})
message("--> Include base path: "${INCLUDE_HOME})

# 4. Load in-house cmake modules (if exists)
message("--> Loading CMake modules:")
if(EXISTS ${CMAKE_MODULE_PATH}/CMakeBoostTest.cmake)
    include(${CMAKE_MODULE_PATH}/CMakeBoostTest.cmake)
endif()

if(EXISTS ${CMAKE_MODULE_PATH}/CMakeAuxFunctions.cmake)
    include(${CMAKE_MODULE_PATH}/CMakeAuxFunctions.cmake)
endif()

# 5. Set CMake environment
set(CMAKE_CXX_STANDARD 17) 
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 6. Set folders
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(PROJECT_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)

# 7. C++ file filter for adding files in cmake's projects
set(CXX_FILE_EXTENSIONS "*.hpp", "*.cpp")

# 8. Set compilation enviroment
include_directories(${INCLUDE_HOME})
add_definitions(-std=gnu++14)

# 7. Add subdirectories
add_subdirectory(src)