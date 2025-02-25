#include "swap.hpp"

Swap::Swap(double notional, double fixedRate, const std::vector<double>& zeroRates, 
           const std::vector<double>& maturities, double frequency)
    : notional_(notional), fixedRate_(fixedRate), zeroRates_(zeroRates), 
      maturities_(maturities), frequency_(frequency) {}

// Cálculo del factor de descuento e^(-rT)
double Swap::discountFactor(double rate, double time) const {
    return std::exp(-rate * time);
}

// Cálculo del tipo forward continuo
double Swap::forwardRate(int i) const {
    if (i == 0) return zeroRates_[0];  // Primer valor
    double Ti = maturities_[i];
    double Ti_1 = maturities_[i - 1];
    double ZCi = zeroRates_[i];
    double ZCi_1 = zeroRates_[i - 1];

    return (ZCi * Ti - ZCi_1 * Ti_1) / (Ti - Ti_1);
}

// Conversión del tipo continuamente compuesto a semianual
double Swap::convertToSemiannual(double continuousRate) const {
    return 2 * (std::exp(continuousRate / 2.0) - 1);
}

// Cálculo del valor presente neto del swap
double Swap::price() const {
    double fixedLegValue = 0.0, floatingLegValue = 0.0;
    int numPeriods = static_cast<int>(maturities_.size());

    for (int i = 0; i < numPeriods; i++) {
        double time = maturities_[i];
        double DF = discountFactor(zeroRates_[i], time);

        // Pagos de la pata fija
        fixedLegValue += (notional_ * fixedRate_ / frequency_) * DF;

        // Pagos de la pata flotante (estimados con tipos forward)
        double forward = forwardRate(i);
        double floatingRate = convertToSemiannual(forward);
        floatingLegValue += (notional_ * floatingRate / frequency_) * DF;
    }

    // Valor nominal descontado
    fixedLegValue += notional_ * discountFactor(zeroRates_.back(), maturities_.back());
    floatingLegValue += notional_ * discountFactor(zeroRates_.back(), maturities_.back());

    return floatingLegValue - fixedLegValue;
}