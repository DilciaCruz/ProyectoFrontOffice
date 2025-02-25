#include "day_count_calculator.hpp"

boost::gregorian::date DayCountCalculator::make_date(const std::string& date) {
    return boost::gregorian::from_string(date);
}