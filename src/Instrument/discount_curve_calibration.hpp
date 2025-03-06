#ifndef DISCOUNT_CURVE_CALIBRATION_HPP
#define DISCOUNT_CURVE_CALIBRATION_HPP

#include "zero_coupon_curve.hpp"
#include "actual_360.hpp"
#include "factory.hpp"
#include "instrument_description.hpp"
#include "bond.hpp"
#include "swap.hpp"
#include <vector>
#include <memory>
#include <map>
#include <string>
#include <boost/date_time/gregorian/gregorian.hpp>

class CurveCalibrator {
public:
    CurveCalibrator(const boost::gregorian::date& baseDate);
    
    // Tomamos los depósitos como bonos cupón cero
    void addDeposit(double rate, int months);
    void addSwap(double rate, int months, int fixedFrequency = 2, int floatingFrequency = 2);
    
    std::shared_ptr<ZeroCouponCurve> calibrate();
    
private:
    boost::gregorian::date baseDate_;
    std::unique_ptr<DayCountCalculator> dayCalculator_;
    std::vector<std::unique_ptr<Instrument>> instruments_;
    std::map<size_t, double> instrumentRates_;
    std::map<size_t, int> maturitiesInMonths_;

    std::vector<boost::gregorian::date> buildPaymentDates(
        const boost::gregorian::date& start,
        const boost::gregorian::date& end,
        int frequency);
    
    double interpolateDiscountFactor(
        double targetYearFraction,
        const std::vector<double>& maturities,
        const std::vector<double>& discountFactors);
};

#endif // DISCOUNT_CURVE_CALIBRATION_HPP