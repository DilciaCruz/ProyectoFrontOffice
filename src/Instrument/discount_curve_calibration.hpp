#ifndef DISCOUNT_CURVE_CALIBRATION_HPP
#define DISCOUNT_CURVE_CALIBRATION_HPP

#include <vector>
#include <memory>
#include <string>
#include <map>
#include <functional>
#include "instrument.hpp"
#include "instrument_description.hpp"
#include "zero_coupon_curve.hpp"
#include "factory.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>

class DiscountCurveCalibration {
public:
    // Constructor with valuation date
    DiscountCurveCalibration(const boost::gregorian::date& valuationDate);

    // Add a calibration instrument with its market price and weight
    void addInstrument(const InstrumentDescription& instrumentDesc, double marketPrice, double weight = 1.0);
    
    // Add multiple instruments from market data
    void addInstrumentsFromMarketData(const std::vector<std::pair<InstrumentDescription, double>>& marketData);
    
    // Calibrate the curve and return the calibrated curve
    std::shared_ptr<ZeroCouponCurve> calibrate(
        const std::vector<double>& initialRatesGuess,
        const std::vector<double>& knots,
        double tolerance = 1e-6,
        int maxIterations = 100,
        bool verbose = false
    );
    
    // Get error metrics from the last calibration
    double getTotalError() const;
    std::vector<double> getIndividualErrors() const;
    
    // Reset the calibration (clear all instruments)
    void reset();
    
private:
    // Objective function for optimization
    double objectiveFunction(const std::vector<double>& zeroRates);
    
    // Update curve with new rates and compute instrument prices
    void updateCurveAndInstrumentPrices(const std::vector<double>& zeroRates, const std::vector<double>& knots);
    
    // Create instrument from description
    std::unique_ptr<Instrument> createInstrument(const InstrumentDescription& desc);
    
    // Return weighted RMSE error
    double calculateError(const std::vector<double>& modelPrices, const std::vector<double>& marketPrices) const;
    
    // Root finding algorithm (Levenberg-Marquardt simplified)
    std::vector<double> optimizeCurve(
        const std::vector<double>& initialRates,
        double tolerance,
        int maxIterations,
        bool verbose
    );
    
    boost::gregorian::date valuationDate_;
    std::vector<InstrumentDescription> instrumentDescriptions_;
    std::vector<double> marketPrices_;
    std::vector<double> modelPrices_;
    std::vector<double> weights_;
    std::vector<double> individualErrors_;
    
    std::shared_ptr<ZeroCouponCurve> currentCurve_;
    double totalError_;
};

#endif // DISCOUNT_CURVE_CALIBRATION_HPP