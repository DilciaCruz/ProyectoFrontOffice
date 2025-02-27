message("** CMakeAuxFunctions ..... Loading")

include(CMakeParseArguments)

function(create_library)
    cmake_parse_arguments(
        PARSED_ARGS 
        "" 
        "NAME" 
        "DEPS" 
        ${ARGN}
    )

    if(NOT PARSED_ARGS_NAME)
        message(FATAL_ERROR "You must provide a NAME argument")
    endif()

    # Buscar archivos fuente en la carpeta actual
    file(GLOB SRC_FILES *.cpp)

    if(NOT SRC_FILES)
        message(FATAL_ERROR "No source files found for ${PARSED_ARGS_NAME}")
    endif()

    # Incluir Boost si está disponible
    if(Boost_FOUND)
        include_directories(${Boost_INCLUDE_DIRS})
        link_directories(${Boost_LIBRARY_DIRS})
    else()
        message(WARNING "Boost not found, continuing without it.")
    endif()

    # Incluir directorio actual para encontrar headers
    include_directories(.)

    # Crear la librería compartida
    add_library(${PARSED_ARGS_NAME} SHARED ${SRC_FILES})

    # Agregar dependencias si existen
    set(DEP_PPRINT "")
    foreach(lib_dep ${PARSED_ARGS_DEPS})
        set(DEP_PPRINT "${DEP_PPRINT} ${lib_dep}")
        target_link_libraries(${PARSED_ARGS_NAME} PRIVATE ${lib_dep})
    endforeach()

    # Mensaje de éxito
    message("+ Created library '${PARSED_ARGS_NAME}' with dependencies: [${DEP_PPRINT}]")
endfunction(create_library)

message("** CMakeAuxFunctions ..... Loaded")