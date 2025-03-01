#include "zero_coupon_curve.hpp"
#include <algorithm>
#include <iomanip>      // For controlling the output format
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
double ZeroCouponCurve::continuousToEffective(double continuousRate, double frequency) const {
    return frequency * (std::exp(continuousRate / frequency) - 1);
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

double ZeroCouponCurve::getSpotRate(double timeToMaturity) const {
    auto it = std::lower_bound(maturities.begin(), maturities.end(), timeToMaturity);
    if (it == maturities.end()) return zeroRates.back();
    size_t index = std::distance(maturities.begin(), it);
    return zeroRates[index];  // Devuelve la tasa cero-cupón interpolada
}
/*double ZeroCouponCurve::forwardRate(double start, double end) const {
    if (i == 0) {
        return zeroRates.front() / 100.0;  // Primer punto es la tasa spot inicial
    }

    double Ti = maturities[i];
    double Ti_1 = maturities[i - 1];
    double ZCi = zeroRates[i] / 100.0;   
    double ZCi_1 = zeroRates[i - 1] / 100.0;
    double RFi = (ZCi * Ti - ZCi_1 * Ti_1) / (Ti - Ti_1);
    
    return RFi;
}*/

double ZeroCouponCurve::forwardRate(double start, double end) const {
    // Validar que las fechas están en el rango de la curva
    if (start < 0.0 || end <= start || end > maturities.back()) {
        throw std::invalid_argument("Fechas fuera del rango de la curva.");
    }

    // Obtener Spot Rate para start y end (usamos la interpolación de getSpotRate)
    double spotStart = getSpotRate(start) / 100.0;  // Convertir de % a decimal
    double spotEnd = getSpotRate(end) / 100.0;

    // Fórmula del tipo forward continuo
    double forwardRate = (spotEnd * end - spotStart * start) / (end - start);

    return forwardRate;
}