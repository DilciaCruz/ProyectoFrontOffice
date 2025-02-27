//#define BOOST_TEST_MODULE FactoryTest
//#include <boost/test/included/unit_test.hpp>
//#include "../factory.hpp"
//#include "../instrument_description.hpp"

//BOOST_AUTO_TEST_CASE(factory_test) {
   // InstrumentDescription desc("Bond");
   // BOOST_CHECK(desc.validate());

   // auto bond = Factory::getInstance().build(desc.getType(), new Thirty360());
   // BOOST_CHECK_EQUAL(bond->price(), 100.0);
//}
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(TestFactory)

BOOST_AUTO_TEST_CASE(SomeTest) {
    BOOST_TEST(1 == 1);
}

BOOST_AUTO_TEST_SUITE_END()