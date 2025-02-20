# Crear la biblioteca Instrument con todos los archivos fuente
add_library(Instrument
    instrument.cpp 
    bond.cpp 
    swap.cpp 
    instrument_description.cpp
    factory.cpp 
    factory_registrator.cpp 
    bond_builder.cpp
    swap_builder.cpp
    day_count_calculator.cpp
    thirty_360.cpp
    actual_360.cpp
)

# Incluir Boost
find_package(Boost REQUIRED)

if(Boost_FOUND)
    message("--> Boost encontrado en: ${Boost_INCLUDE_DIRS}")
    include_directories(${Boost_INCLUDE_DIRS})
else()
    message(FATAL_ERROR "Boost no encontrado en el sistema. Instálalo y verifica las rutas.")
endif()

# Especificar directorios de inclusión para la biblioteca
target_include_directories(Instrument PUBLIC ${Boost_INCLUDE_DIRS})

# Enlazar con Boost
target_link_libraries(Instrument ${Boost_LIBRARIES})

# Agregar el subdirectorio de pruebas unitarias
add_subdirectory(test)