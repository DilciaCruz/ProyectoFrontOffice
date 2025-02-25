#ifndef INSTRUMENT_HPP
#define INSTRUMENT_HPP

#include <memory>
#include <string>

class Instrument {
public:
    virtual ~Instrument() = default;  // Destructor virtual para permitir la herencia

    // Método virtual puro que deben implementar los instrumentos derivados
    virtual double price() const = 0;

    // Opcional: Puedes agregar otros métodos virtuales comunes a todos los instrumentos
};

#endif // INSTRUMENT_HPP