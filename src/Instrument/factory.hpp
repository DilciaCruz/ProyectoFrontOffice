#ifndef FACTORY_HPP
#define FACTORY_HPP

#include <map>
#include <memory>
#include <functional>
#include "instrument.hpp"
#include "instrument_description.hpp"

// Factory con operator() para crear instrumentos directamente.
class Factory {
public:
    using Builder = std::function<std::unique_ptr<Instrument>(const InstrumentDescription&)>;

    static Factory& instance();

    // Registrar un constructor para un tipo específico (Bond o Swap)
    void register_constructor(InstrumentDescription::Type type, const Builder& builder);

    // Este es el operator() que permite crear directamente un instrumento.
    std::unique_ptr<Instrument> operator()(const InstrumentDescription& description) const;

private:
    Factory() = default;

    // Mapea cada tipo (bond, swap) a su función constructora.
    std::map<InstrumentDescription::Type, Builder> builders_;

    InstrumentDescription::Type getBuilderId(const InstrumentDescription& description) const;
};

#endif // FACTORY_HPP