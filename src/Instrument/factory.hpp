#ifndef FACTORY_HPP
#define FACTORY_HPP

#include <map>
#include <memory>
#include <string>
#include <functional>
#include "instrument.hpp"  // Clase base Instrument
#include "instrument_description.hpp"  // Para obtener la descripción del instrumento

class Factory {
public:
    using Builder = std::function<std::unique_ptr<Instrument>(const InstrumentDescription&)>;

    static Factory& instance(); // Singleton

    void register_constructor(const InstrumentDescription::Type& id, const Builder& builder);

    std::unique_ptr<Instrument> operator()(const InstrumentDescription& description) const;

    virtual ~Factory() = default;

private:
    Factory() = default;
    std::map<InstrumentDescription::Type, Builder> buildersMap_;

    // Método privado para obtener el tipo de instrumento desde la descripción
    InstrumentDescription::Type getBuilderId(const InstrumentDescription& description) const;
};

#endif // FACTORY_HPP