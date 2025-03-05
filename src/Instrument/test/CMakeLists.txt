# Incluir las funciones personalizadas para Boost Test
include(${CMAKE_SOURCE_DIR}/cmake-lib/CMakeBoostTest.cmake)

# Mensaje opcional para depuración
message("--> Configurando tests unitarios de Instrument...")

# Definir cada test como un ejecutable separado usando la macro personalizada `boost_test_project`
boost_test_project(NAME test_bond SRCS test_bond.cpp DEPS Instrument)
boost_test_project(NAME test_swap SRCS test_swap.cpp DEPS Instrument)
boost_test_project(NAME test_factory SRCS test_factory.cpp DEPS Instrument)
boost_test_project(NAME test_zerocouponCurveSwap  SRCS test_zerocouponCurveSwap.cpp DEPS Instrument)
boost_test_project(NAME test_zero_coupon_discount SRCS test_zero_coupon_discount.cpp DEPS Instrument)
boost_test_project(NAME test_tir SRCS test_tir.cpp DEPS Instrument)
boost_test_project(NAME test_discount_calibration SRCS test_discount_calibration.cpp DEPS Instrument)