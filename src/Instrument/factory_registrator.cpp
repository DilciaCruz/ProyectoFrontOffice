#ifndef FACTORY_REGISTRATOR_TPP
#define FACTORY_REGISTRATOR_TPP

#include "factory_registrator.hpp"

template<typename BuilderClass>
FactoryRegistrator<BuilderClass>::FactoryRegistrator() {
    Factory::instance().registerBuilder(BuilderClass::getId(), &BuilderClass::build);
}

#endif // FACTORY_REGISTRATOR_TPP