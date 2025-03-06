#include "discount_curve_calibration.hpp"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <stdexcept>
#include <cmath>

// Constructor actualizado
CurveCalibrator::CurveCalibrator(const boost::gregorian::date& baseDate, 
                                InterpolationMethod method)
    : baseDate_(baseDate), 
      dayCalculator_(std::make_unique<Actual_360>()), 
      interpolationMethod_(method) {}

// Tomamos los depósitos como bonos cupón cero
void CurveCalibrator::addDeposit(double rate, int months) {
    boost::gregorian::date maturityDate = baseDate_ + boost::gregorian::months(months);
    double yearFraction = dayCalculator_->compute_daycount(baseDate_, maturityDate) / 360.0;
    
    InstrumentDescription desc(InstrumentDescription::bond);
    desc.notional = 100.0;  // Valor nominal estándar
    desc.issueDate = baseDate_;
    desc.maturity = yearFraction;
    desc.couponRate = 0.0;  // Cupón cero
    desc.frequency = 0.0;   // No hay pagos de cupón
    
    // Temporalmente usamos una curva plana hasta que tengamos la calibrada
    std::vector<double> tempRates = {rate / 100.0};
    std::vector<double> tempMaturities = {yearFraction};
    auto tempCurve = std::make_shared<ZeroCouponCurve>(tempRates, tempMaturities);
    desc.zeroCouponCurve = tempCurve;
    
    // Almacenar la tasa para el proceso de calibración
    instrumentRates_[instruments_.size()] = rate / 100.0;
    maturitiesInMonths_[instruments_.size()] = months;
    
    // Crear el instrumento usando la factoría
    auto instrument = Factory::instance()(desc);
    instruments_.push_back(std::move(instrument));
    
    std::cout << "Depósito agregado: " << months << "m, rate = " << rate << "%, "
              << "vencimiento = " << boost::gregorian::to_iso_extended_string(maturityDate)
              << std::endl;
}

// Método para agregar un swap
void CurveCalibrator::addSwap(double rate, int months, int fixedFrequency, int floatingFrequency) {
    boost::gregorian::date maturityDate = baseDate_ + boost::gregorian::months(months);
    double yearFraction = dayCalculator_->compute_daycount(baseDate_, maturityDate) / 360.0;
    
    InstrumentDescription desc(InstrumentDescription::swap);
    desc.notional = 100.0;  // Valor nominal estándar
    desc.issueDate = baseDate_;
    desc.maturity = yearFraction;
    desc.fixedRate = rate / 100.0;
    desc.fixedFrequency = fixedFrequency;
    desc.floatingFrequency = floatingFrequency;
    desc.initialFixing = rate / 100.0;  // Usamos la misma tasa como inicial
    desc.floatingIndex = "EURIBOR";     // Índice por defecto
    desc.dayCountConvention = "ACT/360";
    
    // Temporalmente usamos una curva plana hasta que tengamos la calibrada
    std::vector<double> tempRates = {rate / 100.0};
    std::vector<double> tempMaturities = {yearFraction};
    auto tempCurve = std::make_shared<ZeroCouponCurve>(tempRates, tempMaturities);
    desc.zeroCouponCurve = tempCurve;
    
    // Almacenar la tasa para el proceso de calibración
    instrumentRates_[instruments_.size()] = rate / 100.0;
    maturitiesInMonths_[instruments_.size()] = months;
    
    // Crear el instrumento usando la factoría
    auto instrument = Factory::instance()(desc);
    instruments_.push_back(std::move(instrument));
    
    std::cout << "Swap agregado: " << months << "m, rate = " << rate << "%, "
              << "vencimiento = " << boost::gregorian::to_iso_extended_string(maturityDate)
              << std::endl;
}

// Método para calibrar la curva
std::shared_ptr<ZeroCouponCurve> CurveCalibrator::calibrate() {
    std::cout << "\nIniciando calibración de curva..." << std::endl;
    
    // Ordenar los instrumentos por vencimiento
    std::vector<size_t> indices(instruments_.size());
    for (size_t i = 0; i < indices.size(); ++i) {
        indices[i] = i;
    }

    std::sort(indices.begin(), indices.end(),
             [this](size_t a, size_t b) { 
                 return maturitiesInMonths_[a] < maturitiesInMonths_[b]; 
             });

    // Crear vectores temporales para instrumentos ordenados
    std::vector<std::unique_ptr<Instrument>> sortedInstruments;
    std::map<size_t, double> sortedRates;
    std::map<size_t, int> sortedMaturities;

    // Copiar en orden ordenado
    for (size_t i = 0; i < indices.size(); ++i) {
        size_t originalIndex = indices[i];
        sortedInstruments.push_back(std::move(instruments_[originalIndex]));
        sortedRates[i] = instrumentRates_[originalIndex];
        sortedMaturities[i] = maturitiesInMonths_[originalIndex];
    }

    // Reemplazar con vectores ordenados
    instruments_ = std::move(sortedInstruments);
    instrumentRates_ = std::move(sortedRates);
    maturitiesInMonths_ = std::move(sortedMaturities);
    
    // Vectores para almacenar resultados
    std::vector<double> zeroRates;
    std::vector<double> maturitiesInYears;
    std::vector<boost::gregorian::date> maturities;
    std::vector<double> discountFactors;
    
    // Calibrar para cada instrumento
    for (size_t i = 0; i < instruments_.size(); ++i) {
        auto& instrument = instruments_[i];
        int months = maturitiesInMonths_[i];
        double rate = instrumentRates_[i];
        
        boost::gregorian::date maturityDate = baseDate_ + boost::gregorian::months(months);
        double yearFraction = dayCalculator_->compute_daycount(baseDate_, maturityDate) / 360.0;
        
        double df = 0.0;
        
        // Verificar el tipo de instrumento
        if (Bond* bond = dynamic_cast<Bond*>(instrument.get())) {
            /* Es un depósito (bono cupón cero) 
             *
             * Fórmula: DF(T) = 1 / (1 + r * T)
             * 
             * Donde:
             * - DF(T) es el factor de descuento para el tiempo T
             * - r es la tasa del depósito (como decimal)
             * - T es la fracción de año hasta el vencimiento
            */
            df = 1.0 / (1.0 + rate * yearFraction);
            
            std::cout << "Calibrado depósito " << months << "m: DF = " 
                      << std::fixed << std::setprecision(6) << df;
        } 
        else if (Swap* swap = dynamic_cast<Swap*>(instrument.get())) {
                        /*
             * CALIBRACIÓN CON SWAPS
             * =====================
             * Para un swap con tasa fija S y pagos en tiempos t_i:
             * 
             * Fórmula: S * Σ(DF(t_i) * Δt_i) + DF(T) = 1
             * 
             * Despejando el último factor de descuento:
             * DF(T) = 1 - S * Σ(DF(t_i) * Δt_i)
             * 
             * Donde:
             * - DF(t_i) son los factores de descuento ya calibrados
             * - S es la tasa fija del swap (como decimal)
             * - Δt_i es el accrual para cada período (típicamente 1/frecuencia)
             * - T es el vencimiento final del swap
             */
            int fixedFrequency = static_cast<int>(swap->getFixedFrequency());
            
            // Construir fechas de pago de la pata fija
            std::vector<boost::gregorian::date> fixedDates = 
                buildPaymentDates(baseDate_, maturityDate, fixedFrequency);
            
            // Calcular suma para el denominador de la ecuación del swap
            double fixedLegSum = 0.0;
            double accrualPerPeriod = 1.0 / fixedFrequency;
            
            for (size_t j = 0; j < fixedDates.size() - 1; ++j) {
                double paymentYearFraction = 
                    dayCalculator_->compute_daycount(baseDate_, fixedDates[j]) / 360.0;
                
                // Encontrar el factor de descuento para esta fecha
                double paymentDF;
                if (j < discountFactors.size()) {
                    paymentDF = discountFactors[j];
                } else {
                    // Interpolar si es necesario
                    paymentDF = interpolateDiscountFactor(
                        paymentYearFraction, maturitiesInYears, discountFactors);
                }
                
                fixedLegSum += paymentDF * accrualPerPeriod;
            }
            
            // Resolver para el último factor de descuento
            // DF(T) = (1 - S * Σ(DF(t_i) * Δt_i)) / (1 + S * Δt_final) 
            df = (1.0 - rate * fixedLegSum) / (1.0 + rate * accrualPerPeriod);
            
            std::cout << "Calibrado swap " << months << "m: DF = " 
                      << std::fixed << std::setprecision(6) << df;
        }
        else {
            throw std::runtime_error("Tipo de instrumento no soportado en la calibración");
        }
        
        discountFactors.push_back(df);
        
        // Calcular tasa zero-coupon equivalente
        // Fórmula: r = -ln(DF(T)) / T
        double zeroRate = -std::log(df) / yearFraction * 100.0;  // En porcentaje
        
        zeroRates.push_back(zeroRate);
        maturitiesInYears.push_back(yearFraction);
        maturities.push_back(maturityDate);
        
        std::cout << ", Zero Rate = " << std::fixed 
                  << std::setprecision(4) << zeroRate << "%" 
                  << std::endl;
    }
    
    // Crear y retornar la curva calibrada
    return std::make_shared<ZeroCouponCurve>(baseDate_, zeroRates, maturities);
}

// Método para construir fechas de pago
std::vector<boost::gregorian::date> CurveCalibrator::buildPaymentDates(
    const boost::gregorian::date& start,
    const boost::gregorian::date& end,
    int frequency) {
    
    std::vector<boost::gregorian::date> dates;
    int monthsPerPeriod = 12 / frequency;
    
    boost::gregorian::date current = start;
    dates.push_back(current);
    
    while (current < end) {
        current = current + boost::gregorian::months(monthsPerPeriod);
        dates.push_back(current);
    }
    
    return dates;
}

// Método de interpolación modificado para soportar log-lineal
double CurveCalibrator::interpolateDiscountFactor(
    double targetYearFraction,
    const std::vector<double>& maturities,
    const std::vector<double>& discountFactors) {
    
    // Encontrar los dos puntos más cercanos
    size_t i = 0;
    while (i < maturities.size() && maturities[i] < targetYearFraction) {
        ++i;
    }
    
    if (i == 0) {
        return discountFactors[0];
    }
    
    if (i == maturities.size()) {
        return discountFactors.back();
    }
    
    // Obtener valores para la interpolación
    double t0 = maturities[i-1];
    double t1 = maturities[i];
    double df0 = discountFactors[i-1];
    double df1 = discountFactors[i];
    
    // Elegir el tipo de interpolación
    if (interpolationMethod_ == InterpolationMethod::LogLinear) {
        /*
         * INTERPOLACIÓN LOG-LINEAL DE FACTORES DE DESCUENTO
         * =================================================
         * Para un tiempo t entre dos puntos conocidos t0 y t1:
         * 
         * Fórmula: DF(t) = exp(ln(DF(t0)) * (t1 - t) / (t1 - t0) + ln(DF(t1)) * (t - t0) / (t1 - t0))
         */
        // Paso 1: Convertir factores de descuento a tasas
        double r0 = -std::log(df0) / t0;
        double r1 = -std::log(df1) / t1;
        
        // Paso 2: Aplicar interpolación log-lineal en las tasas
        double weight1 = (targetYearFraction - t0) / (t1 - t0);
        double weight0 = (t1 - targetYearFraction) / (t1 - t0);
        
        double lnr0 = std::log(r0);
        double lnr1 = std::log(r1);
        
        double lnrT = weight1 * lnr1 + weight0 * lnr0;
        double rT = std::exp(lnrT);
        
        // Paso 3: Convertir la tasa interpolada de vuelta a factor de descuento
        return std::exp(-rT * targetYearFraction);
    } 
    else { // Interpolación lineal (predeterminada)
        /*
         * INTERPOLACIÓN LINEAL DE FACTORES DE DESCUENTO
         * ============================================
         * Para un tiempo t entre dos puntos conocidos t0 y t1:
         * 
         * Fórmula: DF(t) = DF(t0) + (t - t0) * (DF(t1) - DF(t0)) / (t1 - t0)
         */
        return df0 + (targetYearFraction - t0) * (df1 - df0) / (t1 - t0);
    }
}