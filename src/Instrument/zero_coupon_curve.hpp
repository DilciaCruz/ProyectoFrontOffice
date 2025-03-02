#ifndef ZERO_COUPON_CURVE_HPP
#define ZERO_COUPON_CURVE_HPP

#include <vector>
#include <boost/date_time/gregorian/gregorian.hpp>

class ZeroCouponCurve {
public:
    // Constructor para BONOS (maturities en años)
    ZeroCouponCurve(const std::vector<double>& zeroRates, const std::vector<double>& maturities);

    // Constructor para SWAPS (fechas reales)
    ZeroCouponCurve(const boost::gregorian::date& issueDate,
                    const std::vector<double>& zeroRates,
                    const std::vector<boost::gregorian::date>& dates);

    double getDiscountFactor(double accrualFraction) const;
    double getSpotRate(double accrualFraction, int frequency) const;
    double forwardRate(double start, double end) const;
    double computeYearFraction(const boost::gregorian::date& start, const boost::gregorian::date& end) const;
    double continuousToEffective(double continuousRate, double frequency) const;
private:
    void computeDiscountFactors();

    boost::gregorian::date issueDate;
    std::vector<double> zeroRates;
    std::vector<double> maturities;
    std::vector<boost::gregorian::date> dates;  // Solo se usa en swaps
    std::vector<double> discountFactors;
};

#endif