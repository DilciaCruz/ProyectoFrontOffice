#include "bond.hpp"

Bond::Bond(double faceValue, double couponRate, int yearsToMaturity, double marketRate, int frequency)
    : faceValue_(faceValue), couponRate_(couponRate), yearsToMaturity_(yearsToMaturity),
      marketRate_(marketRate), frequency_(frequency) {}

// Factor de descuento e^(-rt)
double Bond::discountFactor(double t) const {
    return std::exp(-marketRate_ * t);
}

// Precio del bono con interés continuamente compuesto
double Bond::price() const {
    double price = 0.0;
    double dt = 1.0 / frequency_;  // Período entre pagos de cupón (en años)
    
    // Sumar los pagos de cupón descontados
    for (int i = 1; i <= yearsToMaturity_ * frequency_; i++) {
        double t = i * dt;  // Tiempo en años
        price += (faceValue_ * couponRate_ * dt) * discountFactor(t);
    }

    // Sumar el valor nominal descontado
    price += faceValue_ * discountFactor(yearsToMaturity_);

    return price;
}