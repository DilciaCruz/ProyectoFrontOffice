#define BOOST_TEST_MODULE BondTest
#include <boost/test/unit_test.hpp>
#include "../bond.hpp"

BOOST_AUTO_TEST_SUITE(BondSuite)

BOOST_AUTO_TEST_CASE(TestBondPrice) {
    double maturity = 2.0;
    double couponRate = 0.06;
    double frequency = 2.0;
    double notional = 100;
    std::vector<double> couponDates = {0.5, 1.0, 1.5, 2.0};

    //  **1. Definir tasas zero-coupon**
    std::vector<double> zeroRates = {5.0, 5.8,6.4,6.8};

    //  **2. Generar los tiempos de vencimiento**
    std::vector<double> maturities(20);
    double start = 0.5;
    for (size_t i = 0; i < maturities.size(); ++i) {
        maturities[i] = start;
        start += 0.5;
    }

    //  **3. Calcular los factores de descuento**
    std::vector<double> zeroPrices;
    for (size_t i = 0; i < maturities.size(); ++i) {
        zeroPrices.push_back(std::exp(-zeroRates[i] / 100.0 * maturities[i]));  // ✅ Conversión correcta
    }

    // **4. Crear la curva de rendimiento con los factores de descuento**
    std::shared_ptr<YieldCurve> myCurve = std::make_shared<YieldCurve>(zeroPrices, maturities);

    // **5. Crear el bono con la curva de rendimiento**
    Bond bond(maturity, couponRate, frequency, notional, couponDates, myCurve);

    // **6. Calcular el precio del bono**
    double computedPrice = bond.price();
  BOOST_TEST_MESSAGE("\n **Ejecutando Test de Precio del Bono**");
    BOOST_TEST_MESSAGE(" - Maturity: " << maturity << " años");
    BOOST_TEST_MESSAGE(" - Coupon Rate: " << couponRate * 100 << "%");
    BOOST_TEST_MESSAGE(" - Frequency: " << frequency);
    BOOST_TEST_MESSAGE(" - Notional: $" << notional);
    BOOST_TEST_MESSAGE(" - Precio Calculado: $" << computedPrice);

    BOOST_TEST(computedPrice > 98.0);
    BOOST_TEST(computedPrice < 98.5);
    
    // **7. Verificar que el precio esté dentro del rango esperado**
    BOOST_TEST(computedPrice > 97.5);
    BOOST_TEST(computedPrice < 99.5);
}

BOOST_AUTO_TEST_SUITE_END()