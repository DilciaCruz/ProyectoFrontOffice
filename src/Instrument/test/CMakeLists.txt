# Habilitar pruebas
enable_testing()

# Crear ejecutable para la prueba de la fábrica
add_executable(FactoryTest test_factory.cpp)
target_link_libraries(FactoryTest Instrument ${Boost_LIBRARIES})
add_test(NAME FactoryTest COMMAND FactoryTest)

# Crear ejecutable para la prueba del cálculo de días
add_executable(DayCountTest test_day_count.cpp)
target_link_libraries(DayCountTest Instrument ${Boost_LIBRARIES})
add_test(NAME DayCountTest COMMAND DayCountTest)