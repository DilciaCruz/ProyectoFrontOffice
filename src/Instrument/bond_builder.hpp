#ifndef BOND_BUILDER_HPP
#define BOND_BUILDER_HPP

#include "bond.hpp"
#include "instrument_description.hpp"

class BondBuilder {
public:
    static std::unique_ptr<Instrument> build(const InstrumentDescription& description);
    static InstrumentDescription::Type getId();
};

#endif // BOND_BUILDER_HPP