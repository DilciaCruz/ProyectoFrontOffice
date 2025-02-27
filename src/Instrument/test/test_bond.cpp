#define BOOST_TEST_MODULE BondTest
#include <boost/test/unit_test.hpp>
#include "../bond.hpp"
#include "../zero_coupon_curve.hpp"
#include "../instrument_description.hpp"
#include "../factory.hpp"
#include "../bond_builder.hpp"
#include "../swap_builder.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>

BOOST_AUTO_TEST_SUITE(BondSuite)

BOOST_AUTO_TEST_CASE(TestBondPrice) {
    boost::gregorian::date issueDate(boost::gregorian::from_simple_string("2024-01-01"));

    // Crear una descripción del instrumento (bond)
    InstrumentDescription bondDescription(InstrumentDescription::bond);
    bondDescription.maturity = 2.0;
    bondDescription.couponRate = 0.06;
    bondDescription.frequency = 2.0;
    bondDescription.notional = 100;
    bondDescription.issueDate = issueDate;
    bondDescription.couponDates = {0.5, 1.0, 1.5, 2.0};

    // Crear la curva de descuento
    std::vector<double> zeroRates = {5.0, 5.8, 6.4, 6.8};
    std::vector<double> maturities = {0.5, 1.0, 1.5, 2.0};
    std::shared_ptr<ZeroCouponCurve> zeroCurve = std::make_shared<ZeroCouponCurve>(zeroRates, maturities);
    
    bondDescription.zeroCouponCurve = zeroCurve;

    // **Registra explícitamente los builders antes de usar Factory**:
    Factory::instance().register_constructor(InstrumentDescription::bond, BondBuilder::build);

    // **Usar Factory para crear el bono**
    std::unique_ptr<Instrument> instrument = Factory::instance()(bondDescription); 

    // **Hacer dynamic_cast de unique_ptr<Instrument> a unique_ptr<Bond>**
    std::unique_ptr<Bond> bond = std::unique_ptr<Bond>(dynamic_cast<Bond*>(instrument.release()));

    if (bond) {  // Verifica si el cast fue exitoso
        // **Calcular el precio del bono**
        double computedPrice = bond->price();

        BOOST_TEST_MESSAGE("Precio del bono calculado: $" << computedPrice);
        BOOST_TEST(computedPrice > 98.0);
        BOOST_TEST(computedPrice < 99.0);

        // **Parámetros de la TIR**
        double tolerance = 1.48E-08;  // Error máximo permitido
        int maxIterations = 50;  // Aumenta iteraciones para mayor precisión
        double initialGuess = 0.08;  // 6.76%

        // **Calcular la TIR**
        double computedYTM = bond->yieldToMaturity(initialGuess, maxIterations, tolerance,computedPrice);

        BOOST_TEST_MESSAGE("TIR calculada: " << computedYTM * 100 << "%");

        // **Verificar que la TIR esté dentro del rango esperado**
        BOOST_TEST(computedYTM > 0.04);
        BOOST_TEST(computedYTM < 0.07);
    } else {
        BOOST_FAIL("El objeto no es de tipo Bond");
    }
}

BOOST_AUTO_TEST_SUITE_END()