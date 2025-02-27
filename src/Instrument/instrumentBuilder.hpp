/*
#ifndef INSTRUMENT_BUILDER_HPP
#define INSTRUMENT_BUILDER_HPP

#include <memory>
#include <string>
#include "instrument.hpp"

class InstrumentBuilder {
public:
    virtual ~InstrumentBuilder() = default;

    // Método virtual puro que las subclases deben sobrescribir
    virtual std::unique_ptr<Instrument> build(const std::string& type) const = 0;
};

#endif // INSTRUMENT_BUILDER_HPP*/