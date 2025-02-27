# CMake generated Testfile for 
# Source directory: /home/dilcia/Documentos/FrontOffice/ProyectoFrontOffice/ProyectoFrontOffice/src/Instrument/test
# Build directory: /home/dilcia/Documentos/FrontOffice/ProyectoFrontOffice/ProyectoFrontOffice/build/src/Instrument/test
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(test_bond.TestBondPrice "/home/dilcia/Documentos/FrontOffice/ProyectoFrontOffice/ProyectoFrontOffice/build/test_bond" "--run_test=BondSuite/TestBondPrice" "--catch_system_error=yes")
set_tests_properties(test_bond.TestBondPrice PROPERTIES  _BACKTRACE_TRIPLES "/home/dilcia/Documentos/FrontOffice/ProyectoFrontOffice/ProyectoFrontOffice/cmake-lib/CMakeBoostTest.cmake;71;add_test;/home/dilcia/Documentos/FrontOffice/ProyectoFrontOffice/ProyectoFrontOffice/src/Instrument/test/CMakeLists.txt;13;boost_test_project;/home/dilcia/Documentos/FrontOffice/ProyectoFrontOffice/ProyectoFrontOffice/src/Instrument/test/CMakeLists.txt;0;")
add_test(test_bond.factory_test "/home/dilcia/Documentos/FrontOffice/ProyectoFrontOffice/ProyectoFrontOffice/build/test_bond" "--run_test=TestFactory/factory_test" "--catch_system_error=yes")
set_tests_properties(test_bond.factory_test PROPERTIES  _BACKTRACE_TRIPLES "/home/dilcia/Documentos/FrontOffice/ProyectoFrontOffice/ProyectoFrontOffice/cmake-lib/CMakeBoostTest.cmake;71;add_test;/home/dilcia/Documentos/FrontOffice/ProyectoFrontOffice/ProyectoFrontOffice/src/Instrument/test/CMakeLists.txt;13;boost_test_project;/home/dilcia/Documentos/FrontOffice/ProyectoFrontOffice/ProyectoFrontOffice/src/Instrument/test/CMakeLists.txt;0;")
add_test(test_bond.SomeTest "/home/dilcia/Documentos/FrontOffice/ProyectoFrontOffice/ProyectoFrontOffice/build/test_bond" "--run_test=TestFactory/SomeTest" "--catch_system_error=yes")
set_tests_properties(test_bond.SomeTest PROPERTIES  _BACKTRACE_TRIPLES "/home/dilcia/Documentos/FrontOffice/ProyectoFrontOffice/ProyectoFrontOffice/cmake-lib/CMakeBoostTest.cmake;71;add_test;/home/dilcia/Documentos/FrontOffice/ProyectoFrontOffice/ProyectoFrontOffice/src/Instrument/test/CMakeLists.txt;13;boost_test_project;/home/dilcia/Documentos/FrontOffice/ProyectoFrontOffice/ProyectoFrontOffice/src/Instrument/test/CMakeLists.txt;0;")
