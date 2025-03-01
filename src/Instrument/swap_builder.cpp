#include "swap_builder.hpp"

std::unique_ptr<Instrument> SwapBuilder::build(const InstrumentDescription& description) {
    description.validate();
    return std::make_unique<Swap>(description);  // Aquí se pasa el descriptor completo
}

InstrumentDescription::Type SwapBuilder::getId() {
    return InstrumentDescription::swap;
}