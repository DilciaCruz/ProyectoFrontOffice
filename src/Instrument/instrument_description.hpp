#ifndef INSTRUMENT_DESCRIPTION_HPP
#define INSTRUMENT_DESCRIPTION_HPP

#include <vector>
#include <memory>
#include <string>
#include <stdexcept>
#include <boost/date_time/gregorian/gregorian.hpp>
#include "zero_coupon_curve.hpp"

struct InstrumentDescription {
    enum Type { bond, swap };
    Type type;

    double maturity = 0.0;
    double notional = 0.0;
    double frequency = 0.0;
    boost::gregorian::date issueDate = boost::gregorian::day_clock::local_day();

    // Solo para bonos
    double couponRate = 0.0;
    std::vector<double> couponDates;

    // Solo para swaps
   // Solo para swaps
    double fixedRate = 0.0;
    double fixedFrequency = 0.0;     // Frecuencia de la pata fija
    double floatingFrequency = 0.0;  // Frecuencia de la pata flotante
    double initialFixing = 0.0;      // Fixing inicial de la tasa flotante
    std::string floatingIndex;       // Ejemplo: "Euribor6M"
    std::string dayCountConvention;  // ACT/360 o 30/360


    std::shared_ptr<ZeroCouponCurve> zeroCouponCurve;

    // Constructor
    InstrumentDescription(Type type_);
    
    // Validaciones
    void validate() const;
};

#endif // INSTRUMENT_DESCRIPTION_HPP