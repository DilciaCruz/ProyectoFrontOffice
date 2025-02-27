#include "factory.hpp"
#include <stdexcept>
#include <iostream>

Factory& Factory::instance() {
    static Factory factory;
    return factory;
}

void Factory::register_constructor(const InstrumentDescription::Type& id, const Builder& builder) {
    buildersMap_[id] = builder;
}

std::unique_ptr<Instrument> Factory::operator()(const InstrumentDescription& description) const {
    auto builder = buildersMap_.find(getBuilderId(description));
    if (builder == buildersMap_.end()) {
        throw std::runtime_error("Invalid instrument type in description");
    }
    return (builder->second)(description);
}

InstrumentDescription::Type Factory::getBuilderId(const InstrumentDescription& description) const {
    return description.type;
}