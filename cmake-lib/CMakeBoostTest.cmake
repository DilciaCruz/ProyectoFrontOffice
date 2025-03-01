message("** CMakeBoostTest ..... Loading")

include(CMakeParseArguments)
set(Boost_USE_STATIC_LIBS ON)

find_package(Boost COMPONENTS unit_test_framework REQUIRED)

enable_testing()

function(boost_test_project)
    cmake_parse_arguments(
        PARSED_ARGS 
        "" 
        "NAME;SRCS" 
        "DEPS" 
        ${ARGN}
    )

    if(NOT PARSED_ARGS_NAME)
        message(FATAL_ERROR "You must provide a NAME argument")
    endif()

    if(NOT PARSED_ARGS_SRCS)
        message(FATAL_ERROR "You must provide at least one source file (SRCS argument)")
    endif()

    message("--> Creating boost test project: ${PARSED_ARGS_NAME} with sources: ${PARSED_ARGS_SRCS}")

    project(${PARSED_ARGS_NAME})
    include_directories(..)

    add_executable(${PARSED_ARGS_NAME} ${PARSED_ARGS_SRCS})
    target_link_libraries(${PARSED_ARGS_NAME} PRIVATE ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

    # Agregar dependencias
    foreach(lib_dep ${PARSED_ARGS_DEPS})
        target_link_libraries(${PARSED_ARGS_NAME} PRIVATE ${lib_dep})
    endforeach()

    # Leer el archivo fuente (único archivo)
    file(READ "${PARSED_ARGS_SRCS}" SOURCE_FILE_CONTENTS)

    if(NOT SOURCE_FILE_CONTENTS)
        message(FATAL_ERROR "Could not read source file: ${PARSED_ARGS_SRCS}")
    endif()

    # Extraer el nombre del test suite
    string(REGEX MATCH "BOOST_AUTO_TEST_SUITE\\( *([A-Za-z_0-9]+) *\\)" FOUND_TESTS_SUITE "${SOURCE_FILE_CONTENTS}")

    if(NOT FOUND_TESTS_SUITE)
        message(FATAL_ERROR "No BOOST_AUTO_TEST_SUITE found in ${PARSED_ARGS_SRCS}")
    endif()

    string(REGEX REPLACE ".*BOOST_AUTO_TEST_SUITE\\( *([A-Za-z_0-9]+) *\\).*" "\\1" TEST_SUITE_NAME "${FOUND_TESTS_SUITE}")

    # Extraer los nombres de los test cases
    string(REGEX MATCHALL "BOOST_AUTO_TEST_CASE\\( *([A-Za-z_0-9]+) *\\)" FOUND_TESTS "${SOURCE_FILE_CONTENTS}")

    foreach(HIT ${FOUND_TESTS})
        string(REGEX REPLACE ".*BOOST_AUTO_TEST_CASE\\( *([A-Za-z_0-9]+) *\\).*" "\\1" TEST_NAME "${HIT}")

        message("Found test: ${PARSED_ARGS_NAME}.${TEST_NAME}")
        add_test(NAME "${PARSED_ARGS_NAME}.${TEST_NAME}"
                 COMMAND ${CMAKE_BINARY_DIR}/${PARSED_ARGS_NAME}
                 --run_test=${TEST_SUITE_NAME}/${TEST_NAME} --catch_system_error=yes)
    endforeach()
endfunction(boost_test_project)

message("** CMakeBoostTest ..... Loaded")