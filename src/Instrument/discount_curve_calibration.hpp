#ifndef DISCOUNT_CURVE_CALIBRATION_HPP
#define DISCOUNT_CURVE_CALIBRATION_HPP

#include "zero_coupon_curve.hpp"
#include "actual_360.hpp"
#include "factory.hpp"
#include "instrument_description.hpp"
#include "bond.hpp"
#include "swap.hpp"
#include <vector>
#include <memory>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <stdexcept>

class CurveCalibrator {
public:
    // Constructor que toma la fecha base para la calibración
    CurveCalibrator(const boost::gregorian::date& baseDate) 
        : baseDate_(baseDate), dayCalculator_(std::make_unique<Actual_360>()) {}
    
    // Método para agregar un depósito (implementado como bono cupón cero)
    void addDeposit(double rate, int months) {
        boost::gregorian::date maturityDate = baseDate_ + boost::gregorian::months(months);
        double yearFraction = dayCalculator_->compute_daycount(baseDate_, maturityDate) / 360.0;
        
        // Crear una descripción para un bono cupón cero
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
    
    // Método para agregar un swap a la calibración
    void addSwap(double rate, int months, int fixedFrequency = 2, int floatingFrequency = 2) {
        boost::gregorian::date maturityDate = baseDate_ + boost::gregorian::months(months);
        double yearFraction = dayCalculator_->compute_daycount(baseDate_, maturityDate) / 360.0;
        
        // Crear una descripción para un swap
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
    std::shared_ptr<ZeroCouponCurve> calibrate() {
        std::cout << "\nIniciando calibración de curva..." << std::endl;
        
        // Reemplazar la función de ordenación con un enfoque más seguro
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
                // Es un depósito (bono cupón cero)
                df = 1.0 / (1.0 + rate * yearFraction);
                
                std::cout << "Calibrado depósito " << months << "m: DF = " 
                          << std::fixed << std::setprecision(6) << df;
            } 
            else if (Swap* swap = dynamic_cast<Swap*>(instrument.get())) {
                // Es un swap
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
                df = (1.0 - rate * fixedLegSum) / (1.0 + rate * accrualPerPeriod);
                
                std::cout << "Calibrado swap " << months << "m: DF = " 
                          << std::fixed << std::setprecision(6) << df;
            }
            else {
                throw std::runtime_error("Tipo de instrumento no soportado en la calibración");
            }
            
            discountFactors.push_back(df);
            
            // Calcular tasa zero-coupon equivalente
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
    
private:
    boost::gregorian::date baseDate_;
    std::unique_ptr<DayCountCalculator> dayCalculator_;
    
    // Vector de instrumentos
    std::vector<std::unique_ptr<Instrument>> instruments_;
    
    // Mapeo de índices a tasas y vencimientos
    std::map<size_t, double> instrumentRates_;
    std::map<size_t, int> maturitiesInMonths_;
    
    // Método para construir fechas de pago
    std::vector<boost::gregorian::date> buildPaymentDates(
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
    
    // Método para interpolar factores de descuento
    double interpolateDiscountFactor(
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
        
        // Interpolación lineal
        double t0 = maturities[i-1];
        double t1 = maturities[i];
        double df0 = discountFactors[i-1];
        double df1 = discountFactors[i];
        
        return df0 + (targetYearFraction - t0) * (df1 - df0) / (t1 - t0);
    }
};

#endif // DISCOUNT_CURVE_CALIBRATION_HPP