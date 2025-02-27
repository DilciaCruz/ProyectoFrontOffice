#ifndef BOND_HPP
#define BOND_HPP

#include <vector>
#include <cmath>
#include <boost/date_time/gregorian/gregorian.hpp>
#include "instrument.hpp"  
#include "zero_coupon_curve.hpp"  

class Bond : public Instrument {
public:
    Bond() = default;  
    Bond(boost::gregorian::date issueDate, double maturity, double couponRate, 
         double frequency, double notional, const std::vector<double>& couponDates, 
         const std::shared_ptr<ZeroCouponCurve>& curve);  

    double price() const;
    double yieldToMaturity(double initialGuess, int maxIterations, double tolerance, double price) const;  

private:
    boost::gregorian::date issueDate;  
    double maturity;
    double couponRate;
    double frequency;
    double notional;
    std::vector<double> couponDates;
    std::shared_ptr<ZeroCouponCurve> zeroCouponCurve;  
};

#endif // BOND_HPP