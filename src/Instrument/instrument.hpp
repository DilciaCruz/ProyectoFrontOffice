#ifndef INSTRUMENT_HPP
#define INSTRUMENT_HPP

#include <memory>
#include <string>

class Instrument {
public:
    virtual ~Instrument() = default;  // Destructor virtual para permitir la herencia

    virtual double price() const = 0;

};

#endif // INSTRUMENT_HPP