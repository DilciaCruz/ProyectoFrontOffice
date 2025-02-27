#ifndef BOND_HPP
#define BOND_HPP

#include <vector>
#include <cmath>
#include "instrument.hpp"  // Asegurar que la clase base está incluida
#include "yield_curve.hpp"
class Bond : public Instrument {
public:
    Bond() = default;  // Prohíbe la creación sin parámetros
    //
    Bond(double maturity, double couponRate, double frequency, double notional, const std::vector<double>& couponDates, const std::shared_ptr<YieldCurve>&curve);
    double price() const;
    double yieldToMaturity(double initialGuess, int maxIterations, double tolerance) const;  // TIR using Newton-Raphson

private:
    double maturity;               // Time to maturity in years 
    double couponRate;             // Annual coupon rate
    double frequency;              // Payment frequency (2.0 for semiannual, 1.0 for annual)
    double notional;               // Notional amount (principal)
    std::vector<double> couponDates;  // Coupon payment dates
    std::shared_ptr<YieldCurve> yieldCurve;  // Yield curve associated with the bond
    double parYield;
};

#endif // BOND_HPP