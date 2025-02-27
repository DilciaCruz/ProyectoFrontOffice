#include "instrument_description.hpp"

// Constructor con valores predeterminados
InstrumentDescription::InstrumentDescription(Type type_) : type(type_) {}

// Validación de datos de entrada
void InstrumentDescription::validate() const {
    if (maturity <= 0) throw std::invalid_argument("Maturity debe ser mayor a 0.");
    if (notional <= 0) throw std::invalid_argument("Notional debe ser positivo.");
    if (frequency <= 0) throw std::invalid_argument("Frecuencia debe ser positiva.");

    if (!zeroCouponCurve) throw std::invalid_argument("ZeroCouponCurve no puede ser nullptr.");

    if (type == bond) {
        if (couponRate < 0 || couponRate > 1) 
            throw std::invalid_argument("Coupon rate fuera de rango (0-1).");
        if (couponDates.empty() && couponRate > 0) 
            throw std::invalid_argument("Bonos con cupón deben tener fechas de pago.");
    } 
    else if (type == swap) {
        if (fixedLegCashFlows.empty() || floatingLegCashFlows.empty()) 
            throw std::invalid_argument("Swap debe tener ambas patas definidas.");
        if (fixedLegCashFlows.size() != floatingLegCashFlows.size()) 
            throw std::invalid_argument("Las patas del swap deben tener la misma cantidad de flujos.");
        if (fixedRate < 0 || fixedRate > 1) 
            throw std::invalid_argument("Fixed rate fuera de rango (0-1).");
        if (floatingIndex.empty()) 
            throw std::invalid_argument("Floating index no definido.");
    }
}