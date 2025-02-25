#include "actual_360.hpp"

short Actual_360::compute_daycount(const std::string& from, const std::string& to) {
    boost::gregorian::date from_date = boost::gregorian::from_string(from);
    boost::gregorian::date to_date = boost::gregorian::from_string(to);
    return compute_daycount(from_date, to_date);
}

short Actual_360::compute_daycount(const boost::gregorian::date& from,
                                   const boost::gregorian::date& to) const {
    return (to - from).days();
}