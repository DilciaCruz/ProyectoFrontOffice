#include <vector>
#include <cmath>
/**
 * Class representing a yield curve for zero-coupon bonds.
 */
class YieldCurve { 
private:
    std::vector<double> zeroCouponBondPrices;  // Discount factors for zero-coupon bonds
    std::vector<double> maturities;            // Maturity dates in years

public:
    YieldCurve();
    /**
     * Constructor to initialize discount factors and maturity dates.
     * @param zeroPrices Vector of discount factors.
     * @param maturityDates Vector of maturity dates in years.
     */
    YieldCurve(const std::vector<double>& zeroPrices, const std::vector<double>& maturityDates);

    /**
     * Get the discount factor for a given accrual fraction.
     * @param accrualFraction The time to maturity in years.
     * @return The corresponding discount factor.
     */
    double getZero(double accrualFraction) const;
};
