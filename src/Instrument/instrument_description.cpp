#include "instrument_description.hpp"

// Constructor
InstrumentDescription::InstrumentDescription(Type type_) : type(type_) {}

// Validación de consistencia de datos
void InstrumentDescription::validate() const {
    if (maturity <= 0) throw std::invalid_argument("Maturity debe ser mayor a 0.");
    if (notional <= 0) throw std::invalid_argument("Notional debe ser positivo.");
    if (!zeroCouponCurve) throw std::invalid_argument("ZeroCouponCurve no puede ser nullptr.");

    if (type == bond) {
        if (couponRate < 0 || couponRate > 1) 
            throw std::invalid_argument("Coupon rate fuera de rango (0-1).");
        if (couponDates.empty()) 
            throw std::invalid_argument("El bono debe tener fechas de cupón.");
        if (frequency <= 0) 
            throw std::invalid_argument("Frecuencia de cupón inválida.");
    } 
    else if (type == swap) {
        if (fixedRate < 0 || fixedRate > 1) 
            throw std::invalid_argument("Fixed rate fuera de rango (0-1).");
        if (fixedFrequency <= 0 || floatingFrequency <= 0) 
            throw std::invalid_argument("Frecuencias de patas inválidas.");
        if (floatingIndex.empty()) 
            throw std::invalid_argument("Floating index no definido.");
        if (initialFixing < 0) 
            throw std::invalid_argument("Fixing inicial inválido.");
        if (dayCountConvention != "ACT/360" && dayCountConvention != "30/360")
            throw std::invalid_argument("Convención de día inválida.");
    }
}