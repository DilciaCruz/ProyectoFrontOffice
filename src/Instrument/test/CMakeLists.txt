# Incluir las funciones personalizadas para Boost Test
include(${CMAKE_SOURCE_DIR}/cmake-lib/CMakeBoostTest.cmake)

# Mensaje opcional para depuración
message("--> Configurando tests unitarios de Instrument...")

# Definir cada test como un ejecutable separado usando la macro personalizada `boost_test_project`
boost_test_project(NAME test_bond SRCS test_bond.cpp DEPS Instrument)
boost_test_project(NAME test_swap SRCS test_swap.cpp DEPS Instrument)
boost_test_project(NAME test_factory SRCS test_factory.cpp DEPS Instrument)
