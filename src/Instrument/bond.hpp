#ifndef BOND_HPP
#define BOND_HPP

#include <vector>
#include <cmath>
#include "instrument.hpp"  // Asegurar que la clase base está incluida

class Bond : public Instrument {
public:
    Bond(double faceValue, double couponRate, int yearsToMaturity, double marketRate, int frequency);

    double price() const;  // Precio del bono con interés continuamente compuesto
    double yieldToMaturity() const;  // TIR usando Newton-Raphson

private:
    double faceValue_;       // Valor nominal (ej. 1000)
    double couponRate_;      // Tasa de cupón anual (ej. 5% -> 0.05)
    int yearsToMaturity_;    // Plazo del bono en años
    double marketRate_;      // Tasa del mercado (descuento continuamente compuesto)
    int frequency_;          // Frecuencia de pagos por año (ej. 1=anual, 2=semestral, 4=trimestral)

    double discountFactor(double t) const;  // Factor de descuento e^(-rt)
};

#endif // BOND_HPP