#ifndef SWAP_HPP
#define SWAP_HPP

#include "instrument.hpp"
#include "instrument_description.hpp"
#include "zero_coupon_curve.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>

class Swap : public Instrument {
public:
    Swap(const InstrumentDescription& description);

    double price() const;

private:
    double notional_;
    double fixedRate_;
    double fixedFrequency_;
    double floatingFrequency_;
    double initialFloatingRate_;
    std::string floatingIndex_;
    std::string dayCountConvention_;
    boost::gregorian::date issueDate_;
    double maturity_;

    std::shared_ptr<ZeroCouponCurve> zeroCouponCurve_;

    double accrualFraction(const boost::gregorian::date& start, const boost::gregorian::date& end) const;
};

#endif // SWAP_HPP