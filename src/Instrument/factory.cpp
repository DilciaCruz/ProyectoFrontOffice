#include "factory.hpp"
#include <stdexcept>

Factory& Factory::instance() {
    static Factory factory;
    return factory;
}

void Factory::register_constructor(InstrumentDescription::Type type, const Builder& builder) {
    builders_[type] = builder;
}

std::unique_ptr<Instrument> Factory::operator()(const InstrumentDescription& description) const {
    auto it = builders_.find(getBuilderId(description));
    if (it == builders_.end()) {
        throw std::runtime_error("No hay builder registrado para el tipo de instrumento.");
    }
    return (it->second)(description);  // Ejecuta la función que construye el objeto
}

InstrumentDescription::Type Factory::getBuilderId(const InstrumentDescription& description) const {
    return description.type;  // Devuelve directamente el tipo (bond o swap)
}