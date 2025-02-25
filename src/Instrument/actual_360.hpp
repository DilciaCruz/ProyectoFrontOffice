#ifndef ACTUAL_360_HPP
#define ACTUAL_360_HPP

#include "day_count_calculator.hpp"

class Actual_360 : public DayCountCalculator {
public:
    short compute_daycount(const std::string& from, const std::string& to);
    short compute_daycount(const boost::gregorian::date& from,
                           const boost::gregorian::date& to) const override;
};

#endif // ACTUAL_360_HPP