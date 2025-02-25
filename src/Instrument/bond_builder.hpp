#ifndef BOND_BUILDER_HPP
#define BOND_BUILDER_HPP

#include "instrumentBuilder.hpp"
#include "bond.hpp"

class BondBuilder : public InstrumentBuilder {
public:
    std::unique_ptr<Instrument> build(const std::string& type) const override;
};

#endif // BOND_BUILDER_HPP