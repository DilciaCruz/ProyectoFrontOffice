#include "factory_registrator.hpp"
#include "bond_builder.hpp"
#include "swap_builder.hpp"

template<typename BuilderClass>
FactoryRegistrator<BuilderClass>::FactoryRegistrator() {
    // Registra el constructor del Builder (usando el tipo de instrumento como ID)
    Factory::instance().register_constructor(BuilderClass::getId(), &BuilderClass::build);
}

// Especializaciones para BondBuilder y SwapBuilder
template class FactoryRegistrator<BondBuilder>;
//template class FactoryRegistrator<SwapBuilder>;