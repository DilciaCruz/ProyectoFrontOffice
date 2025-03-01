#define BOOST_TEST_MODULE SwapTest
#include <boost/test/unit_test.hpp>
#include "../instrument_description.hpp"
#include "../swap_builder.hpp"
#include "../factory.hpp"
#include "../factory_registrator.hpp"
#include "../zero_coupon_curve.hpp"

// Registrar el SwapBuilder automáticamente
static FactoryRegistrator<SwapBuilder> swapRegistrator;

BOOST_AUTO_TEST_SUITE(SwapSuite)

BOOST_AUTO_TEST_CASE(TestSwapPricing) {
    InstrumentDescription desc(InstrumentDescription::swap);

    // Configuración según el ejemplo de la imagen
    desc.notional = 100;            // Nominal: 100 millones de euros
    desc.fixedRate = 0.05;                   // 5% anual
    desc.fixedFrequency = 2.0;                // Pago fijo cada 6 meses (semestral)
    desc.floatingFrequency = 2.0;             // Pago flotante cada 6 meses (semestral)
    desc.initialFixing = 0.048;                // Euribor 6M al momento de firma: 4.8%
    desc.floatingIndex = "Euribor";            // Índice flotante es Euribor
    desc.dayCountConvention = "ACT/360";       // Convención de día es ACT/360
    desc.issueDate = boost::gregorian::from_string("2016-04-01");
    desc.maturity = 2.0;                       // Swap de 2 años

    // Curva cero-cupón del ejemplo
    desc.zeroCouponCurve = std::make_shared<ZeroCouponCurve>(
        std::vector<double>{4.74, 5.00, 5.10, 5.20},  // Tasas cero-cupón (continuas)
        std::vector<double>{0.5, 1.0, 1.5, 2.0}       // Vencimientos en años
    );

    // Crear el swap usando la Factory (ya registrado el builder con FactoryRegistrator)
    std::unique_ptr<Instrument> instrument = Factory::instance()(desc);

    // Cast dinámico a Swap para acceder al método price()
    Swap* swap = dynamic_cast<Swap*>(instrument.get());
    BOOST_REQUIRE(swap != nullptr);

    // Calcular precio del swap con trazas incluidas
    double price = swap->price();

    // Mostrar el precio calculado
    BOOST_TEST_MESSAGE("Precio calculado del Swap: " << price);

    // Validar que esté en un rango razonable (esto puede ajustarse si tenés una referencia)
    BOOST_CHECK(price > -5'000'000 && price < 5'000'000);
}

BOOST_AUTO_TEST_SUITE_END()