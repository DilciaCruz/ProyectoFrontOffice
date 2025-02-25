#ifndef SWAP_HPP
#define SWAP_HPP

#include <vector>
#include <cmath>
#include <memory>

class Swap: public Instrument {
public:
    Swap(double notional, double fixedRate, const std::vector<double>& zeroRates, 
         const std::vector<double>& maturities, double frequency);

    double price() const;  // Valor presente del swap

private:
    double notional_;                   // Monto nocional
    double fixedRate_;                   // Tasa fija
    std::vector<double> zeroRates_;      // Tasas cero cupón continuamente compuestas
    std::vector<double> maturities_;     // Fechas de vencimiento
    double frequency_;                   // Frecuencia de pagos (ej. 2 para semestral)

    double discountFactor(double rate, double time) const;  // Factor de descuento
    double forwardRate(int i) const;  // Cálculo del tipo forward
    double convertToSemiannual(double continuousRate) const;  // Conversión a tasa semianual
};

#endif // SWAP_HPP