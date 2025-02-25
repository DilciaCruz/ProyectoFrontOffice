#ifndef THIRTY_360_HPP
#define THIRTY_360_HPP

#include "day_count_calculator.hpp"

class Thirty_360 : public DayCountCalculator {
public:
    short compute_daycount(const std::string& from, const std::string& to);
    short compute_daycount(const boost::gregorian::date& from,
                           const boost::gregorian::date& to) const override;
    static short compute_daycount(short years, short months, short days_from, short days_to);
};

#endif // THIRTY_360_HPP