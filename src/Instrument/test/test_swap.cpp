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

    desc.notional = 100;
    desc.fixedRate = 0.05;
    desc.fixedFrequency = 2.0;
    desc.floatingFrequency = 2.0;
    desc.initialFixing = 0.048;
    desc.floatingIndex = "Euribor";
    desc.dayCountConvention = "ACT/360";
    desc.issueDate = boost::gregorian::from_string("2016-04-01");
    desc.maturity = 2.0;

    desc.zeroCouponCurve = std::make_shared<ZeroCouponCurve>(
        desc.issueDate,
        std::vector<double>{4.74, 5.00, 5.10, 5.20},
        std::vector<boost::gregorian::date>{
            boost::gregorian::date(2016, 10, 1),
            boost::gregorian::date(2017, 4, 1),
            boost::gregorian::date(2017, 10, 1),
            boost::gregorian::date(2018, 4, 1)
        }
    );

    std::unique_ptr<Instrument> instrument = Factory::instance()(desc);

    Swap* swap = dynamic_cast<Swap*>(instrument.get());
    BOOST_REQUIRE(swap != nullptr);

    double price = swap->price();

    BOOST_TEST_MESSAGE("Precio calculado del Swap: " << price * 1000000);
    BOOST_CHECK(price > -5.0 && price < 5.0);
}

BOOST_AUTO_TEST_SUITE_END()