#include "factory.hpp"
#include <stdexcept>
#include <iostream>

Factory& Factory::instance() {
    static Factory factory;
    return factory;
}

void Factory::registerBuilder(const std::string& type, Builder builder) {
    builders_[type] = builder;
}

std::unique_ptr<Instrument> Factory::create(const std::string& type) const {
    auto it = builders_.find(type);
    if (it == builders_.end()) {
        throw std::runtime_error("Unknown instrument type: " + type);
    }
    return (it->second)(type);
}
#include "factory.hpp"

int main() {
    try {
        auto swap = Factory::instance().create("swap");
        auto bond = Factory::instance().create("bond");

        // Uso de los instrumentos creados
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}