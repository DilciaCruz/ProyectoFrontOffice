#include "swap_builder.hpp"

std::unique_ptr<Instrument> SwapBuilder::build(const std::string& type) const {
    return std::make_unique<Swap>();
}