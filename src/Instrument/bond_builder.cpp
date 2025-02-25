#include "bond_builder.hpp"

std::unique_ptr<Instrument> BondBuilder::build(const std::string& type) const {
    return std::make_unique<Bond>();
}