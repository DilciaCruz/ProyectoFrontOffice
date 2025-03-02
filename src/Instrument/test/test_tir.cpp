#define BOOST_TEST_MODULE BondYieldTest
#include <boost/test/unit_test.hpp>
#include "../bond.hpp"
#include "../zero_coupon_curve.hpp"
#include "../instrument_description.hpp"
#include "../factory.hpp"
#include "../bond_builder.hpp"
#include "../factory_registrator.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <iomanip>

// Registrar el BondBuilder automáticamente
static FactoryRegistrator<BondBuilder> bondRegistrator;

BOOST_AUTO_TEST_SUITE(BondYieldSuite)

BOOST_AUTO_TEST_CASE(TestBondYieldCalculation) {
    boost::gregorian::date issueDate(boost::gregorian::from_simple_string("2024-01-01"));

    // Crear descripción del bono
    InstrumentDescription bondDescription(InstrumentDescription::bond);
    bondDescription.maturity = 2.0;
    bondDescription.couponRate = 0.06;
    bondDescription.frequency = 2.0;     // Semestral
    bondDescription.notional = 100;
    bondDescription.issueDate = issueDate;
    bondDescription.couponDates = {0.5, 1.0, 1.5, 2.0};

    // Curva cero cupón
    std::vector<double> zeroRates = {5.0, 5.8, 6.4, 6.8};
    std::vector<double> maturities = {0.5, 1.0, 1.5, 2.0};
    std::shared_ptr<ZeroCouponCurve> zeroCurve = std::make_shared<ZeroCouponCurve>(zeroRates, maturities);
    bondDescription.zeroCouponCurve = zeroCurve;

    // Crear el bono usando la Factory
    std::unique_ptr<Instrument> instrument = Factory::instance()(bondDescription);
    Bond* bond = dynamic_cast<Bond*>(instrument.get());

    BOOST_REQUIRE_MESSAGE(bond != nullptr, "El instrumento creado no es un bono válido.");

    // Parámetros Newton-Raphson
    double tolerance = 1.48E-08;
    int maxIterations = 50;
    double initialGuess = 0.05;
    double marketPrice = 98.39;  // Precio observado de mercado

    // Calcular la TIR
    double computedYTM = bond->yieldToMaturity(initialGuess, maxIterations, tolerance, marketPrice);

    // Mostrar resultado formateado directamente desde el test
    std::cout << std::fixed << std::setprecision(2);
    BOOST_TEST_MESSAGE(">>> TIR calculada: " << computedYTM * 100 << "%");

    // Validar rango esperado (esto puede ajustarse según expectativas reales)
    BOOST_CHECK(computedYTM > 0.04);
    BOOST_CHECK(computedYTM < 0.07);

    // Comparar con el valor esperado de 6.75%
    BOOST_CHECK_CLOSE(computedYTM * 100, 6.75, 0.1);
}

BOOST_AUTO_TEST_SUITE_END()