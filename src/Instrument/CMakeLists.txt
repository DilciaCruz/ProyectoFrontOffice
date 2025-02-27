include(${CMAKE_SOURCE_DIR}/cmake-lib/CMakeAuxFunctions.cmake)

# Crear la librería Instrument
create_library(NAME Instrument DEPS Boost::unit_test_framework)

# Agregar subdirectorio de pruebas
add_subdirectory(test)