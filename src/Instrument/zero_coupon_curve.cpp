#include "zero_coupon_curve.hpp"
#include <algorithm>

ZeroCouponCurve::ZeroCouponCurve(const std::vector<double>& zeroRates, const std::vector<double>& maturities)
    : zeroRates(zeroRates), maturities(maturities) {
    computeDiscountFactors();  // Calcula los factores de descuento al inicializar
}

void ZeroCouponCurve::computeDiscountFactors() {
    discountFactors.resize(maturities.size());
    for (size_t i = 0; i < maturities.size(); ++i) {
        discountFactors[i] = std::exp(-zeroRates[i] / 100.0 * maturities[i]);
    }
}

double ZeroCouponCurve::getSpotRate(double timeToMaturity) const {
    auto it = std::lower_bound(maturities.begin(), maturities.end(), timeToMaturity);
    if (it == maturities.end()) return zeroRates.back();
    size_t index = std::distance(maturities.begin(), it);
    return zeroRates[index];  // Devuelve la tasa cero-cupón interpolada
}

double ZeroCouponCurve::getDiscountFactor(double accrualFraction) const {
    if (accrualFraction <= maturities.front()) return discountFactors.front();
    if (accrualFraction >= maturities.back()) return discountFactors.back();

    auto it = std::lower_bound(maturities.begin(), maturities.end(), accrualFraction);
    size_t index = std::distance(maturities.begin(), it);

    if (index == 0) return discountFactors.front();
    
    // Interpolación lineal
    double x0 = maturities[index - 1], x1 = maturities[index];
    double y0 = discountFactors[index - 1], y1 = discountFactors[index];

    return y0 + (accrualFraction - x0) * (y1 - y0) / (x1 - x0);
}