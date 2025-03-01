#ifndef SWAP_BUILDER_HPP
#define SWAP_BUILDER_HPP

#include "swap.hpp"
#include "instrument_description.hpp"

class SwapBuilder {
public:
    static std::unique_ptr<Instrument> build(const InstrumentDescription& description);
    static InstrumentDescription::Type getId();
};

#endif // SWAP_BUILDER_HPP