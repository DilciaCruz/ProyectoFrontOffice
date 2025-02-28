#include "thirty_360.hpp"

short Thirty_360::compute_daycount(const std::string& from, const std::string& to) {
    auto from_date = boost::gregorian::from_string(from);
    auto to_date = boost::gregorian::from_string(to);
    return compute_daycount(from_date, to_date);
}

short Thirty_360::compute_daycount(const boost::gregorian::date& from,
                                   const boost::gregorian::date& to) const {
    auto from_date = from.year_month_day();
    auto to_date = to.year_month_day();
    short years = to_date.year - from_date.year;
    short months = to_date.month - from_date.month;
    return compute_daycount(years, months, from_date.day, to_date.day);
}

short Thirty_360::compute_daycount(short years, short months, short days_from, short days_to) {
    return (360 * years) + (30 * (months - 1)) +
           std::max<short>(0, 30 - days_from) +
           std::min<short>(30, days_to);
}