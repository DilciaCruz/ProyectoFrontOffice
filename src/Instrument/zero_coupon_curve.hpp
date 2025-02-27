#ifndef ZERO_COUPON_CURVE_HPP
#define ZERO_COUPON_CURVE_HPP

#include <vector>
#include <cmath>

class ZeroCouponCurve {
public:
    ZeroCouponCurve(const std::vector<double>& zeroRates, const std::vector<double>& maturities);

    double getSpotRate(double timeToMaturity) const;  // Devuelve tasa cero-cupón interpolada
    double getDiscountFactor(double timeToMaturity) const;  // Devuelve factor de descuento

private:
    std::vector<double> zeroRates;   // Tasas cero-cupón
    std::vector<double> maturities;  // Vencimientos en años
    std::vector<double> discountFactors; // Factores de descuento (internos)
    
    void computeDiscountFactors();  // Convierte tasas a factores de descuento
};

#endif // ZERO_COUPON_CURVE_HPP