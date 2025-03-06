#define BOOST_TEST_MODULE DiscountCurveCalibrationTest
#include <boost/test/included/unit_test.hpp>
#include "../discount_curve_calibration.hpp"
#include "../zero_coupon_curve.hpp"
#include "../factory.hpp"
#include "../bond_builder.hpp"
#include "../swap_builder.hpp"
#include "../factory_registrator.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <iostream>
#include <iomanip>

static FactoryRegistrator<BondBuilder> bondRegistrator;
static FactoryRegistrator<SwapBuilder> swapRegistrator;

BOOST_AUTO_TEST_SUITE(TestDiscountCurveCalibration)

BOOST_AUTO_TEST_CASE(TestDiscountCurveCalibrationWithDepositsAndSwaps) {
    // Fecha inicial: 1/4/2016
    boost::gregorian::date baseDate(2016, 4, 1);
    
    // Crear el calibrador de curvas
    CurveCalibrator calibrator(baseDate);
    
    // Agregar los instrumentos del ejercicio
    calibrator.addDeposit(5.0, 6);     // Depósito 6m r = 5%
    calibrator.addSwap(5.5, 12);       // Swap 12m r = 5.5%
    calibrator.addSwap(6.0, 18);       // Swap 18m r = 6%
    calibrator.addSwap(6.4, 24);       // Swap 2y r = 6.4%
    
    // Calibrar la curva
    auto curve = calibrator.calibrate();
    
    // Mostrar los resultados
    std::cout << "\nCurva de descuento calibrada:" << std::endl;
    std::cout << "-------------------------" << std::endl;
    
    // Verificar factores de descuento para fechas clave
    boost::gregorian::date date6m = baseDate + boost::gregorian::months(6);
    boost::gregorian::date date12m = baseDate + boost::gregorian::months(12);
    boost::gregorian::date date18m = baseDate + boost::gregorian::months(18);
    boost::gregorian::date date24m = baseDate + boost::gregorian::months(24);
    
    double yf6m = curve->computeYearFraction(baseDate, date6m);
    double yf12m = curve->computeYearFraction(baseDate, date12m);
    double yf18m = curve->computeYearFraction(baseDate, date18m);
    double yf24m = curve->computeYearFraction(baseDate, date24m);
    
    double df6m = curve->getDiscountFactor(yf6m);
    double df12m = curve->getDiscountFactor(yf12m);
    double df18m = curve->getDiscountFactor(yf18m);
    double df24m = curve->getDiscountFactor(yf24m);
    
    std::cout << "6m  - DF: " << std::fixed << std::setprecision(6) << df6m << std::endl;
    std::cout << "12m - DF: " << std::fixed << std::setprecision(6) << df12m << std::endl;
    std::cout << "18m - DF: " << std::fixed << std::setprecision(6) << df18m << std::endl;
    std::cout << "24m - DF: " << std::fixed << std::setprecision(6) << df24m << std::endl;
    
    // Validar los resultados (valores esperados aproximados)
    BOOST_CHECK_CLOSE(df6m, 0.975213, 0.1);
    BOOST_CHECK_CLOSE(df12m, 0.947135, 0.1);
    BOOST_CHECK_CLOSE(df18m, 0.914883, 0.1);
    BOOST_CHECK_CLOSE(df24m, 0.881016, 0.1);

}

BOOST_AUTO_TEST_SUITE_END()