cmake_minimum_required(VERSION 3.10)

project(ProyectoFrontOffice)

# 1. Define carpetas base
set(CMAKE_BINARY_DIR ${PROJECT_SOURCE_DIR}/build)
set(INCLUDE_HOME ${PROJECT_SOURCE_DIR}/src)
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake-lib)

# 2. Buscar Boost sin rutas fijas
find_package(Boost REQUIRED)

if(Boost_FOUND)
    message("--> Boost encontrado en: ${Boost_INCLUDE_DIRS}")
    include_directories(${Boost_INCLUDE_DIRS})
else()
    message(FATAL_ERROR "Boost no encontrado en el sistema. Instálalo y verifica las rutas.")
endif()

# 3. Imprimir información del entorno
message("--> CMake extra functions: ${CMAKE_MODULE_PATH}")
message("--> Binaries: ${CMAKE_BINARY_DIR}")
message("--> Include base path: ${INCLUDE_HOME}")

# 4. Cargar módulos de CMake (si existen)
message("--> Loading CMake modules:")
if(EXISTS ${CMAKE_MODULE_PATH}/CMakeBoostTest.cmake)
    include(${CMAKE_MODULE_PATH}/CMakeBoostTest.cmake)
endif()

if(EXISTS ${CMAKE_MODULE_PATH}/CMakeAuxFunctions.cmake)
    include(${CMAKE_MODULE_PATH}/CMakeAuxFunctions.cmake)
endif()

# 5. Configuración del compilador
set(CMAKE_CXX_STANDARD 17) # Usa C++17 o superior
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 6. Definir rutas de salida
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

# 7. Agregar subdirectorios del código fuente
add_subdirectory(src)