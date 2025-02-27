include(${CMAKE_SOURCE_DIR}/cmake-lib/CMakeBoostTest.cmake)

# Buscar Boost
find_package(Boost REQUIRED COMPONENTS unit_test_framework)

if(Boost_FOUND)
    message("--> Boost encontrado en: ${Boost_INCLUDE_DIRS}")
else()
    message(FATAL_ERROR "Boost no encontrado. Instálalo y verifica las rutas.")
endif()

# Crear la prueba usando la función `boost_test_project`
boost_test_project(NAME test_bond DEPS Instrument)