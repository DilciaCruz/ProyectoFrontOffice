/*#include "swap_builder.hpp"

std::unique_ptr<Instrument> SwapBuilder::build(const InstrumentDescription& description) {
    return std::make_unique<Swap>(description.fixedLegCashFlows, description.floatingLegCashFlows,
                                  description.fixedRate, description.floatingIndex, description.notional);
}

InstrumentDescription::Type SwapBuilder::getId() {
    return InstrumentDescription::swap;
}*/