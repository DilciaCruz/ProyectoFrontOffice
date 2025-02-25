#ifndef SWAP_BUILDER_HPP
#define SWAP_BUILDER_HPP

#include "instrumentBuilder.hpp"
#include "swap.hpp"

class SwapBuilder : public InstrumentBuilder {
public:
    std::unique_ptr<Instrument> build(const std::string& type) const override;  // Ahora coincide con la base
};

#endif // SWAP_BUILDER_HPP