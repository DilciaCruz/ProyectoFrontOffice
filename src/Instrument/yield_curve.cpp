#include "yield_curve.hpp"
/**
 * Constructor por defecto: inicializa vectores vacíos.
 */
YieldCurve::YieldCurve() : zeroCouponBondPrices{}, maturities{} {}
    /**
     * Constructor to initialize discount factors and maturity dates.
     * @param zeroPrices Vector of discount factors.
     * @param maturityDates Vector of maturity dates in years.
     */
    YieldCurve::YieldCurve(const std::vector<double>& zeroPrices, const std::vector<double>& maturityDates)
        : zeroCouponBondPrices(zeroPrices), maturities(maturityDates) {}
  /**
     * Get the discount factor for a given accrual fraction.
     * @param accrualFraction The time to maturity in years.
     * @return The corresponding discount factor.
     */
    double YieldCurve::getZero(double accrualFraction) const {
    if (accrualFraction <= maturities.front()) return zeroCouponBondPrices.front();
    if (accrualFraction >= maturities.back()) return zeroCouponBondPrices.back();

    auto it = std::lower_bound(maturities.begin(), maturities.end(), accrualFraction);
    size_t index = std::distance(maturities.begin(), it);

    if (index == 0) return zeroCouponBondPrices.front();
    
    // Interpolación lineal
    double x0 = maturities[index - 1], x1 = maturities[index];
    double y0 = zeroCouponBondPrices[index - 1], y1 = zeroCouponBondPrices[index];

    return y0 + (accrualFraction - x0) * (y1 - y0) / (x1 - x0);
}
    /*double YieldCurve::getZero(double accrualFraction) const {
        auto it = std::lower_bound(maturities.begin(), maturities.end(), accrualFraction);
        // Calculate zero-coupon bond prices using discount rates
        if (it == maturities.end()) return zeroCouponBondPrices.back();  // Return last factor if not found
        size_t index = std::distance(maturities.begin(), it);            // Get the corresponding index
        return zeroCouponBondPrices[index];                              // Return the discount factor
    }*/