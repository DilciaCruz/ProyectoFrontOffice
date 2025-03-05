# CMake generated Testfile for 
# Source directory: /home/andreswo/Github/ProyectoFrontOffice/src/Instrument/test
# Build directory: /home/andreswo/Github/ProyectoFrontOffice/build/src/Instrument/test
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(test_bond.TestBondPrice "/home/andreswo/Github/ProyectoFrontOffice/build/test_bond" "--run_test=BondSuite/TestBondPrice" "--catch_system_error=yes")
set_tests_properties(test_bond.TestBondPrice PROPERTIES  _BACKTRACE_TRIPLES "/home/andreswo/Github/ProyectoFrontOffice/cmake-lib/CMakeBoostTest.cmake;63;add_test;/home/andreswo/Github/ProyectoFrontOffice/src/Instrument/test/CMakeLists.txt;8;boost_test_project;/home/andreswo/Github/ProyectoFrontOffice/src/Instrument/test/CMakeLists.txt;0;")
add_test(test_swap.TestSwapPricing "/home/andreswo/Github/ProyectoFrontOffice/build/test_swap" "--run_test=SwapSuite/TestSwapPricing" "--catch_system_error=yes")
set_tests_properties(test_swap.TestSwapPricing PROPERTIES  _BACKTRACE_TRIPLES "/home/andreswo/Github/ProyectoFrontOffice/cmake-lib/CMakeBoostTest.cmake;63;add_test;/home/andreswo/Github/ProyectoFrontOffice/src/Instrument/test/CMakeLists.txt;9;boost_test_project;/home/andreswo/Github/ProyectoFrontOffice/src/Instrument/test/CMakeLists.txt;0;")
add_test(test_factory.SomeTest "/home/andreswo/Github/ProyectoFrontOffice/build/test_factory" "--run_test=TestFactory/SomeTest" "--catch_system_error=yes")
set_tests_properties(test_factory.SomeTest PROPERTIES  _BACKTRACE_TRIPLES "/home/andreswo/Github/ProyectoFrontOffice/cmake-lib/CMakeBoostTest.cmake;63;add_test;/home/andreswo/Github/ProyectoFrontOffice/src/Instrument/test/CMakeLists.txt;10;boost_test_project;/home/andreswo/Github/ProyectoFrontOffice/src/Instrument/test/CMakeLists.txt;0;")
add_test(test_zerocouponCurveSwap.TestZerocouponCurveSwap "/home/andreswo/Github/ProyectoFrontOffice/build/test_zerocouponCurveSwap" "--run_test=TestZero/TestZerocouponCurveSwap" "--catch_system_error=yes")
set_tests_properties(test_zerocouponCurveSwap.TestZerocouponCurveSwap PROPERTIES  _BACKTRACE_TRIPLES "/home/andreswo/Github/ProyectoFrontOffice/cmake-lib/CMakeBoostTest.cmake;63;add_test;/home/andreswo/Github/ProyectoFrontOffice/src/Instrument/test/CMakeLists.txt;11;boost_test_project;/home/andreswo/Github/ProyectoFrontOffice/src/Instrument/test/CMakeLists.txt;0;")
add_test(test_zero_coupon_discount.TestZeroCouponDiscounts "/home/andreswo/Github/ProyectoFrontOffice/build/test_zero_coupon_discount" "--run_test=ZeroCouponDiscountSuite/TestZeroCouponDiscounts" "--catch_system_error=yes")
set_tests_properties(test_zero_coupon_discount.TestZeroCouponDiscounts PROPERTIES  _BACKTRACE_TRIPLES "/home/andreswo/Github/ProyectoFrontOffice/cmake-lib/CMakeBoostTest.cmake;63;add_test;/home/andreswo/Github/ProyectoFrontOffice/src/Instrument/test/CMakeLists.txt;12;boost_test_project;/home/andreswo/Github/ProyectoFrontOffice/src/Instrument/test/CMakeLists.txt;0;")
add_test(test_tir.TestBondYieldCalculation "/home/andreswo/Github/ProyectoFrontOffice/build/test_tir" "--run_test=BondYieldSuite/TestBondYieldCalculation" "--catch_system_error=yes")
set_tests_properties(test_tir.TestBondYieldCalculation PROPERTIES  _BACKTRACE_TRIPLES "/home/andreswo/Github/ProyectoFrontOffice/cmake-lib/CMakeBoostTest.cmake;63;add_test;/home/andreswo/Github/ProyectoFrontOffice/src/Instrument/test/CMakeLists.txt;13;boost_test_project;/home/andreswo/Github/ProyectoFrontOffice/src/Instrument/test/CMakeLists.txt;0;")
add_test(test_discount_calibration.TestDiscountCurveCalibration "/home/andreswo/Github/ProyectoFrontOffice/build/test_discount_calibration" "--run_test=DiscountCurveCalibrationSuite/TestDiscountCurveCalibration" "--catch_system_error=yes")
set_tests_properties(test_discount_calibration.TestDiscountCurveCalibration PROPERTIES  _BACKTRACE_TRIPLES "/home/andreswo/Github/ProyectoFrontOffice/cmake-lib/CMakeBoostTest.cmake;63;add_test;/home/andreswo/Github/ProyectoFrontOffice/src/Instrument/test/CMakeLists.txt;14;boost_test_project;/home/andreswo/Github/ProyectoFrontOffice/src/Instrument/test/CMakeLists.txt;0;")
