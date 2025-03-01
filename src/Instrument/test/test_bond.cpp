#define BOOST_TEST_MODULE BondTest
#include <boost/test/unit_test.hpp>
#include "../bond.hpp"
#include "../zero_coupon_curve.hpp"
#include "../instrument_description.hpp"
#include "../factory.hpp"
#include "../bond_builder.hpp"
#include "../factory_registrator.hpp"
#include "day_count_calculator.hpp"

// Registrar el BondBuilder automáticamente
static FactoryRegistrator<BondBuilder> bondRegistrator;

BOOST_AUTO_TEST_SUITE(BondSuite)

BOOST_AUTO_TEST_CASE(TestBondPrice) {
    boost::gregorian::date issueDate(boost::gregorian::from_simple_string("2024-01-01"));

    // Crear descripción del bono
    InstrumentDescription bondDescription(InstrumentDescription::bond);
    bondDescription.maturity = 2.0;
    bondDescription.couponRate = 0.06;
    bondDescription.frequency = 2.0;     // Semestral
    bondDescription.notional = 100;
    bondDescription.issueDate = issueDate;
    bondDescription.couponDates = {0.5, 1.0, 1.5, 2.0};

    // Crear la curva de descuento
    std::vector<double> zeroRates = {5.0, 5.8, 6.4, 6.8};
    std::vector<double> maturities = {0.5, 1.0, 1.5, 2.0};
    std::shared_ptr<ZeroCouponCurve> zeroCurve = std::make_shared<ZeroCouponCurve>(zeroRates, maturities);
    bondDescription.zeroCouponCurve = zeroCurve;

    // Crear bono usando la Factory (ya registra el builder automáticamente)
    std::unique_ptr<Instrument> instrument = Factory::instance()(bondDescription);

    // Hacer dynamic_cast a Bond
    std::unique_ptr<Bond> bond(dynamic_cast<Bond*>(instrument.release()));

    if (bond) {
        // Calcular el precio
        double computedPrice = bond->price();

        BOOST_TEST_MESSAGE("Precio del bono calculado: $" << computedPrice);
        BOOST_TEST(computedPrice > 98.0);
        BOOST_TEST(computedPrice < 99.0);

        // Parámetros para Newton-Raphson
        double tolerance = 1.48E-08;
        int maxIterations = 50;
        double initialGuess = 0.08;

        // Calcular TIR (YTM)
        double computedYTM = bond->yieldToMaturity(initialGuess, maxIterations, tolerance, computedPrice);

        BOOST_TEST_MESSAGE("TIR calculada: " << computedYTM * 100 << "%");

        // Validar rango esperado de la TIR
        BOOST_TEST(computedYTM > 0.04);
        BOOST_TEST(computedYTM < 0.07);
    } else {
        BOOST_FAIL("El objeto construido no es un Bond.");
    }
}

BOOST_AUTO_TEST_SUITE_END()