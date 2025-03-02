#include "zero_coupon_curve.hpp"
#include <cmath>
#include <algorithm>
#include <stdexcept>

// Constructor para bonos (maturities en años)
ZeroCouponCurve::ZeroCouponCurve(const std::vector<double>& zeroRates, const std::vector<double>& maturities)
    : zeroRates(zeroRates), maturities(maturities) {
    computeDiscountFactors();
}

// Constructor para swaps (fechas reales)
ZeroCouponCurve::ZeroCouponCurve(const boost::gregorian::date& issueDate,
                                 const std::vector<double>& zeroRates,
                                 const std::vector<boost::gregorian::date>& dates)
    : issueDate(issueDate), zeroRates(zeroRates), dates(dates) {
    maturities.resize(dates.size());
    for (size_t i = 0; i < dates.size(); ++i) {
        maturities[i] = computeYearFraction(issueDate, dates[i]);
    }
    computeDiscountFactors();
}

double ZeroCouponCurve::computeYearFraction(const boost::gregorian::date& start, const boost::gregorian::date& end) const {
    return (end - start).days() / 360.0;
}

void ZeroCouponCurve::computeDiscountFactors() {
    discountFactors.resize(maturities.size());
    for (size_t i = 0; i < maturities.size(); ++i) {
        double zcRate = zeroRates[i] / 100.0;
        discountFactors[i] = std::exp(-zcRate * maturities[i]);
    }
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

double ZeroCouponCurve::getSpotRate(double accrualFraction, int frequency) const {
    if (accrualFraction <= maturities.front()) {
        double zcRate = zeroRates.front() / 100.0;
        return frequency * (std::exp(zcRate / frequency) - 1) * 100.0;
    }

    auto it = std::lower_bound(maturities.begin(), maturities.end(), accrualFraction);
    size_t index = std::distance(maturities.begin(), it);

    if (index == 0) return zeroRates.front();

    if (maturities[index] == accrualFraction) {
        // Caso fecha exacta: aplicar forward directo
        double Ti = maturities[index];
        double Ti_1 = maturities[index - 1];
        double ZCi = zeroRates[index] / 100.0;
        double ZCi_1 = zeroRates[index - 1] / 100.0;

        double RF = (ZCi * Ti - ZCi_1 * Ti_1) / (Ti - Ti_1);
        return frequency * (std::exp(RF / frequency) - 1) * 100.0;
    } else {
        // Fecha intermedia: interpolar DF y recalcular rate
        double DF = getDiscountFactor(accrualFraction);
        double effectiveRate = (1.0 - DF) / accrualFraction;
        return effectiveRate * 100.0;  // Spot interpolado
    }
}
double ZeroCouponCurve::forwardRate(double start, double end) const {
    double DFstart = getDiscountFactor(start);
    double DFend = getDiscountFactor(end);

    return -std::log(DFend / DFstart) / (end - start);
}
double ZeroCouponCurve::continuousToEffective(double continuousRate, double frequency) const {
    return frequency * (std::exp(continuousRate / frequency) - 1);
}