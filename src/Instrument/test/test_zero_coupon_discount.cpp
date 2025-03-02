#define BOOST_TEST_MODULE ZeroCouponDiscountTest
#include <boost/test/unit_test.hpp>
#include "../zero_coupon_curve.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <vector>
#include <iostream>
#include <iomanip>

BOOST_AUTO_TEST_SUITE(ZeroCouponDiscountSuite)

BOOST_AUTO_TEST_CASE(TestZeroCouponDiscounts) {
    // Fechas y curva cero-cupón (igual a la usada en el test del bono)
    std::vector<double> zeroRates = {5.0, 5.8, 6.4, 6.8};  // Tasas cero-cupón en %
    std::vector<double> maturities = {0.5, 1.0, 1.5, 2.0}; // Maturities en años

    // Crear la curva cero cupón
    ZeroCouponCurve zeroCurve(zeroRates, maturities);

    // Mostrar tabla de factores de descuento
    std::cout << "\n>>> Factores de Descuento según la Curva Cero Cupón:\n";
    std::cout << "Maturity (Años) | Zero Rate (%) | Discount Factor\n";
    std::cout << "-------------------------------------------------\n";

    for (size_t i = 0; i < maturities.size(); ++i) {
        double zeroRate = zeroRates[i];  // Tasa cero-cupón en ese maturity
        double df = zeroCurve.getDiscountFactor(maturities[i]);

        std::cout << std::setw(15) << maturities[i] << " | "
                  << std::setw(13) << zeroRate << " | "
                  << std::setw(16) << std::fixed << std::setprecision(5) << df << "\n";

        // Validar con BOOST (si querés, es opcional)
        if (i == 0) {
            BOOST_CHECK_CLOSE(df, std::exp(-0.05 * 0.5), 0.1);
        } else if (i == 1) {
            BOOST_CHECK_CLOSE(df, std::exp(-0.058 * 1.0), 0.1);
        } else if (i == 2) {
            BOOST_CHECK_CLOSE(df, std::exp(-0.064 * 1.5), 0.1);
        } else if (i == 3) {
            BOOST_CHECK_CLOSE(df, std::exp(-0.068 * 2.0), 0.1);
        }
    }

    std::cout << "-------------------------------------------------\n";
}

BOOST_AUTO_TEST_SUITE_END()