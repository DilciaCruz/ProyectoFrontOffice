#include "factory_registrator.hpp"
#include "bond_builder.hpp"
#include "swap_builder.hpp"

template<>
FactoryRegistrator<BondBuilder>::FactoryRegistrator() {
    Factory::instance().register_constructor(BondBuilder::getId(), &BondBuilder::build);
}

template<>
FactoryRegistrator<SwapBuilder>::FactoryRegistrator() {
    Factory::instance().register_constructor(SwapBuilder::getId(), &SwapBuilder::build);
}

// Definir explícitamente las especializaciones
template class FactoryRegistrator<BondBuilder>;
template class FactoryRegistrator<SwapBuilder>;