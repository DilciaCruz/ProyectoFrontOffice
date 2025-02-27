#include "bond_builder.hpp"

std::unique_ptr<Instrument> BondBuilder::build(const InstrumentDescription& description) {
    return std::make_unique<Bond>(description.issueDate, description.maturity, description.couponRate,
                                  description.frequency, description.notional, description.couponDates, description.zeroCouponCurve);
}

InstrumentDescription::Type BondBuilder::getId() {
    return InstrumentDescription::bond;
}