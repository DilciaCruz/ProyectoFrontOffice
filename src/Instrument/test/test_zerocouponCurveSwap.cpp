#define BOOST_TEST_MODULE ZeroCouponCurveTest
#include <boost/test/included/unit_test.hpp>
#include "../zero_coupon_curve.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <iostream>
#include <iomanip>

// Función utilitaria para imprimir fechas
std::string formatDate(const boost::gregorian::date& date) {
    return boost::gregorian::to_iso_extended_string(date);
}
BOOST_AUTO_TEST_SUITE(TestZero)

BOOST_AUTO_TEST_CASE(TestZerocouponCurveSwap) {

    using namespace boost::gregorian;

    date issueDate(2016, 4, 1);
    double frequency = 2;
    std::vector<double> zeroRates = {4.74, 5.00, 5.10, 5.20};
    std::vector<date> dates = {
        date(2016, 10, 3),
        date(2017, 4, 3),
        date(2017, 10, 2),
        date(2018, 4, 2)
    };

    ZeroCouponCurve curve(issueDate, zeroRates, dates);

    std::cout << "Test - Zero Coupon Curve (con fechas y Boost.DateTime)\n";
    std::cout << "------------------------------------------------------\n";

    for (size_t i = 0; i < dates.size(); ++i) {
        double yearFraction = curve.computeYearFraction(issueDate, dates[i]);
        double df = curve.getDiscountFactor(yearFraction);
        double spotRate = curve.getSpotRate(yearFraction,frequency);

        std::cout << "Fecha: " << formatDate(dates[i])
                  << " | Fracción de año (ACT/360): " << std::fixed << std::setprecision(6) << yearFraction
                  << " | DF: " << std::fixed << std::setprecision(4) << df
                  << " | Spot Rate: " << std::fixed << std::setprecision(2) << spotRate << "%\n";

        // Validaciones BOOST
        if (i == 1) {
            BOOST_CHECK_CLOSE(df, 0.9512, 0.1);  // 03/04/2017
        } else if (i == 3) {
            BOOST_CHECK_CLOSE(df, 0.8999, 0.1);  // 02/04/2018
        }
    }

    // Fecha intermedia (para validar interpolación)
    date interpolatedDate(2017, 1, 1);  // Entre 03/10/2016 y 03/04/2017
    double yearFractionInterp = curve.computeYearFraction(issueDate, interpolatedDate);
    double dfInterp = curve.getDiscountFactor(yearFractionInterp);
    double spotInterp = curve.getSpotRate(yearFractionInterp,frequency);

    std::cout << "\nFecha Interpolada: " << formatDate(interpolatedDate)
              << " | Fracción de año (ACT/360): " << std::fixed << std::setprecision(6) << yearFractionInterp
              << " | DF: " << std::fixed << std::setprecision(4) << dfInterp
              << " | Spot Rate: " << std::fixed << std::setprecision(2) << spotInterp << "%\n";

    // Validaciones BOOST de la interpolación
    BOOST_CHECK_CLOSE(dfInterp, 0.9632, 0.2);
    BOOST_CHECK_CLOSE(spotInterp, 4.80, 0.2);
}

BOOST_AUTO_TEST_SUITE_END()

