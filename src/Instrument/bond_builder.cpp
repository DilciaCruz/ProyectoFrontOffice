#include "bond_builder.hpp"

std::unique_ptr<Instrument> BondBuilder::build(const InstrumentDescription& description) {
    return std::make_unique<Bond>(description);  
}

InstrumentDescription::Type BondBuilder::getId() {
    return InstrumentDescription::bond;
}