#ifndef DAY_COUNT_CALCULATOR_HPP
#define DAY_COUNT_CALCULATOR_HPP

#include <boost/date_time/gregorian/gregorian.hpp>
#include <string>

class DayCountCalculator {
public:
    virtual ~DayCountCalculator() = default;

    static boost::gregorian::date make_date(const std::string& date);

    // Método virtual puro (debe ser implementado en las subclases)
    virtual short compute_daycount(const boost::gregorian::date& from,
                                   const boost::gregorian::date& to) const = 0;
};

#endif // DAY_COUNT_CALCULATOR_HPP