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
#include <stdexcept>

static FactoryRegistrator<BondBuilder> bondRegistrator;
static FactoryRegistrator<SwapBuilder> swapRegistrator;

// Función utilitaria para imprimir fechas
std::string formatDate(const boost::gregorian::date& date) {
    std::ostringstream oss;
    oss << date.year() << "-" 
        << std::setw(2) << std::setfill('0') << date.month() << "-"
        << std::setw(2) << std::setfill('0') << date.day();
    return oss.str();
}

// Función para imprimir una curva completa
void printCurve(const std::shared_ptr<ZeroCouponCurve>& curve, 
                const boost::gregorian::date& baseDate, 
                const std::vector<int>& monthsAhead) {
    
    std::cout << "\nPlazos  | Fecha       | Year Frac | DF        | Zero Rate (%)\n";
    std::cout << "--------------------------------------------------------\n";

    for (int months : monthsAhead) {
        boost::gregorian::date futureDate = baseDate + boost::gregorian::months(months);
        double yf = curve->computeYearFraction(baseDate, futureDate);
        double df = curve->getDiscountFactor(yf);
        double rate = -std::log(df) / yf * 100; // Convertir a tasa en %
        
        std::cout << std::setw(7) << std::left << std::to_string(months) + "m" << "| "
                  << formatDate(futureDate) << " | "
                  << std::fixed << std::setprecision(6) << std::setw(9) << yf << " | "
                  << std::setw(9) << df << " | "
                  << std::setw(9) << rate << "\n";
    }
    std::cout << std::endl;
}

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
    calibrator.addSwap(6.4, 24);       // Swap 2a r = 6.4%
    
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
    
    // Validar los resultados con los obtenidos en el Excel
    BOOST_CHECK_CLOSE(df6m, 0.975213, 0.1);
    BOOST_CHECK_CLOSE(df12m, 0.946419, 0.1);
    BOOST_CHECK_CLOSE(df18m, 0.913681, 0.1);
    BOOST_CHECK_CLOSE(df24m, 0.879470, 0.1);
    
    // Mostrar la curva completa con más detalles
    printCurve(curve, baseDate, {3, 6, 9, 12, 15, 18, 21, 24});
}

BOOST_AUTO_TEST_CASE(TestCurveInterpolation) {
    // Prueba de interpolación entre puntos de calibración
    boost::gregorian::date baseDate(2016, 4, 1);
    CurveCalibrator calibrator(baseDate);
    
    calibrator.addDeposit(4.5, 6);   // Depósito 6m r = 4.5%
    calibrator.addDeposit(5.0, 12);  // Depósito 12m r = 5.0%
    
    auto curve = calibrator.calibrate();
    
    // Verificar un punto intermedio (9 meses)
    boost::gregorian::date date9m = baseDate + boost::gregorian::months(9);
    double yf9m = curve->computeYearFraction(baseDate, date9m);
    double df9m = curve->getDiscountFactor(yf9m);
    
    std::cout << "\nPrueba de interpolación:" << std::endl;
    std::cout << "9m - DF: " << std::fixed << std::setprecision(6) << df9m << std::endl;
    
    // El factor de descuento a 9m debería estar entre los de 6m y 12m
    double yf6m = curve->computeYearFraction(baseDate, baseDate + boost::gregorian::months(6));
    double yf12m = curve->computeYearFraction(baseDate, baseDate + boost::gregorian::months(12));
    double df6m = curve->getDiscountFactor(yf6m);
    double df12m = curve->getDiscountFactor(yf12m);
    
    // El DF a 9m debe ser menor que a 6m y mayor que a 12m
    BOOST_CHECK_LT(df9m, df6m);
    BOOST_CHECK_GT(df9m, df12m);
}

BOOST_AUTO_TEST_CASE(TestEmptyCurveCalibration) {
    boost::gregorian::date baseDate(2016, 4, 1);
    CurveCalibrator calibrator(baseDate);
    
    // Intentar calibrar sin instrumentos debería fallar
    std::cout << "*** ESTO DEBERÍA FALLAR ***" << std::endl;
    BOOST_CHECK_THROW(calibrator.calibrate(), std::runtime_error);
}

BOOST_AUTO_TEST_SUITE_END()