#include "discount_curve_calibration.hpp"
#include <algorithm>
#include <numeric>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <stdexcept>

DiscountCurveCalibration::DiscountCurveCalibration(const boost::gregorian::date& valuationDate)
    : valuationDate_(valuationDate), totalError_(0.0) {
}

void DiscountCurveCalibration::addInstrument(const InstrumentDescription& instrumentDesc, double marketPrice, double weight) {
    instrumentDescriptions_.push_back(instrumentDesc);
    marketPrices_.push_back(marketPrice);
    weights_.push_back(weight);
    modelPrices_.push_back(0.0);
    individualErrors_.push_back(0.0);
}

void DiscountCurveCalibration::addInstrumentsFromMarketData(
    const std::vector<std::pair<InstrumentDescription, double>>& marketData
) {
    for (const auto& [desc, price] : marketData) {
        addInstrument(desc, price);
    }
}

std::shared_ptr<ZeroCouponCurve> DiscountCurveCalibration::calibrate(
    const std::vector<double>& initialRatesGuess,
    const std::vector<double>& knots,
    double tolerance,
    int maxIterations,
    bool verbose
) {
    if (instrumentDescriptions_.empty()) {
        throw std::runtime_error("No calibration instruments provided");
    }
    
    if (initialRatesGuess.size() != knots.size()) {
        throw std::runtime_error("Initial rates and knots must have the same size");
    }
    
    if (verbose) {
        std::cout << "\n=== Starting Discount Curve Calibration ===\n";
        std::cout << "Calibrating to " << instrumentDescriptions_.size() << " instruments\n";
        std::cout << "Initial curve points: " << knots.size() << "\n";
    }
    
    // Optimize the curve parameters
    std::vector<double> calibratedRates = optimizeCurve(initialRatesGuess, tolerance, maxIterations, verbose);
    
    // Final update of the curve and prices
    updateCurveAndInstrumentPrices(calibratedRates, knots);
    
    // Calculate final error
    totalError_ = calculateError(modelPrices_, marketPrices_);
    
    // Calculate individual errors
    for (size_t i = 0; i < modelPrices_.size(); ++i) {
        individualErrors_[i] = std::abs(modelPrices_[i] - marketPrices_[i]) / marketPrices_[i] * 100.0;
    }
    
    if (verbose) {
        // Print final results
        std::cout << "\n=== Calibration Results ===\n";
        std::cout << "Total RMSE Error: " << totalError_ << "\n\n";
        
        std::cout << "Calibrated Zero Rates:\n";
        std::cout << "-----------------------\n";
        std::cout << "Maturity | Zero Rate (%)\n";
        
        for (size_t i = 0; i < knots.size(); ++i) {
            std::cout << std::fixed << std::setprecision(2) 
                      << std::setw(8) << knots[i] << " | " 
                      << std::setw(8) << calibratedRates[i] << "\n";
        }
        
        std::cout << "\nInstrument Fit:\n";
        std::cout << "------------------------------------------------------------------------------\n";
        std::cout << "Instrument | Market Price | Model Price | Abs Diff | Relative Error (%)\n";
        
        for (size_t i = 0; i < instrumentDescriptions_.size(); ++i) {
            double absDiff = modelPrices_[i] - marketPrices_[i];
            std::cout << std::fixed 
                      << std::setw(10) << (i+1) << " | " 
                      << std::setprecision(4) << std::setw(12) << marketPrices_[i] << " | "
                      << std::setw(11) << modelPrices_[i] << " | "
                      << std::setw(8) << absDiff << " | "
                      << std::setprecision(2) << std::setw(8) << individualErrors_[i] << "%\n";
        }
        
        std::cout << "==============================================================================\n";
    }
    
    return currentCurve_;
}

double DiscountCurveCalibration::getTotalError() const {
    return totalError_;
}

std::vector<double> DiscountCurveCalibration::getIndividualErrors() const {
    return individualErrors_;
}

void DiscountCurveCalibration::reset() {
    instrumentDescriptions_.clear();
    marketPrices_.clear();
    modelPrices_.clear();
    weights_.clear();
    individualErrors_.clear();
    totalError_ = 0.0;
}

double DiscountCurveCalibration::objectiveFunction(const std::vector<double>& zeroRates) {
    return totalError_;
}

void DiscountCurveCalibration::updateCurveAndInstrumentPrices(
    const std::vector<double>& zeroRates, 
    const std::vector<double>& knots
) {
    // Create new curve with calibrated rates
    currentCurve_ = std::make_shared<ZeroCouponCurve>(zeroRates, knots);
    
    // Recalculate prices for all instruments
    modelPrices_.resize(instrumentDescriptions_.size());
    
    for (size_t i = 0; i < instrumentDescriptions_.size(); ++i) {
        // Create a copy of the instrument description
        InstrumentDescription desc = instrumentDescriptions_[i];
        
        // Update the description with the current curve
        desc.zeroCouponCurve = currentCurve_;
        
        // Create instrument using the factory
        std::unique_ptr<Instrument> instrument = Factory::instance()(desc);
        
        // Calculate and store the model price
        modelPrices_[i] = instrument->price();
    }
}

std::unique_ptr<Instrument> DiscountCurveCalibration::createInstrument(const InstrumentDescription& desc) {
    return Factory::instance()(desc);
}

double DiscountCurveCalibration::calculateError(
    const std::vector<double>& modelPrices, 
    const std::vector<double>& marketPrices
) const {
    double sumSquaredError = 0.0;
    double sumWeights = 0.0;
    
    for (size_t i = 0; i < modelPrices.size(); ++i) {
        double error = (modelPrices[i] - marketPrices[i]) / marketPrices[i];
        sumSquaredError += weights_[i] * error * error;
        sumWeights += weights_[i];
    }
    
    return std::sqrt(sumSquaredError / sumWeights);
}

std::vector<double> DiscountCurveCalibration::optimizeCurve(
    const std::vector<double>& initialRates,
    double tolerance,
    int maxIterations,
    bool verbose
) {
    std::vector<double> rates = initialRates;
    std::vector<double> knots(rates.size());
    for (size_t i = 0; i < rates.size(); ++i) {
        knots[i] = (i + 1.0) / rates.size() * 10.0; // Example knots distribution 
    }
    
    // Initial evaluation
    updateCurveAndInstrumentPrices(rates, knots);
    double error = calculateError(modelPrices_, marketPrices_);
    
    if (verbose) {
        std::cout << "Initial error: " << error << "\n";
    }
    
    double lambda = 0.01;  // Levenberg-Marquardt damping parameter
    int iteration = 0;
    
    while (iteration < maxIterations) {
        // Store current state
        double prevError = error;
        std::vector<double> prevRates = rates;
        
        // Compute Jacobian (partial derivatives)
        std::vector<std::vector<double>> jacobian(marketPrices_.size(), std::vector<double>(rates.size(), 0.0));
        
        for (size_t j = 0; j < rates.size(); ++j) {
            // Perturb rate j
            double h = 0.0001 * std::max(1.0, std::abs(rates[j]));
            rates[j] += h;
            
            // Calculate new prices with perturbed rate
            updateCurveAndInstrumentPrices(rates, knots);
            
            // Compute derivatives
            for (size_t i = 0; i < marketPrices_.size(); ++i) {
                jacobian[i][j] = (modelPrices_[i] - marketPrices_[i]) / h;
            }
            
            // Restore rate
            rates[j] = prevRates[j];
        }
        
        // Restore original prices
        updateCurveAndInstrumentPrices(prevRates, knots);
        
        // Compute gradient and approximate Hessian (J^T * J)
        std::vector<double> gradient(rates.size(), 0.0);
        std::vector<std::vector<double>> hessian(rates.size(), std::vector<double>(rates.size(), 0.0));
        
        for (size_t i = 0; i < marketPrices_.size(); ++i) {
            double residual = modelPrices_[i] - marketPrices_[i];
            
            for (size_t j = 0; j < rates.size(); ++j) {
                gradient[j] += residual * jacobian[i][j] * weights_[i];
                
                for (size_t k = 0; k < rates.size(); ++k) {
                    hessian[j][k] += jacobian[i][j] * jacobian[i][k] * weights_[i];
                }
            }
        }
        
        // Apply Levenberg-Marquardt damping
        for (size_t j = 0; j < rates.size(); ++j) {
            hessian[j][j] += lambda * hessian[j][j];
        }
        
        // Solve linear system to get delta
        std::vector<double> delta(rates.size(), 0.0);
        
        // Simple Gauss-Jordan elimination (simplified for brevity)
        // In a real implementation, use a proper linear solver or numerical library
        for (size_t j = 0; j < rates.size(); ++j) {
            delta[j] = -gradient[j] / hessian[j][j];
        }
        
        // Update rates
        for (size_t j = 0; j < rates.size(); ++j) {
            rates[j] += delta[j];
            
            // Enforce bounds on rates (avoid negative or extreme values)
            rates[j] = std::max(0.01, std::min(20.0, rates[j]));
        }
        
        // Evaluate new error
        updateCurveAndInstrumentPrices(rates, knots);
        error = calculateError(modelPrices_, marketPrices_);
        
        // Check for convergence
        if (std::abs(error - prevError) < tolerance) {
            break;
        }
        
        // Adjust lambda based on success/failure
        if (error < prevError) {
            lambda *= 0.5;  // Decrease damping (more Newton-like)
        } else {
            lambda *= 2.0;  // Increase damping (more gradient descent-like)
            rates = prevRates;  // Revert to previous rates
        }
        
        if (verbose && (iteration % 5 == 0 || iteration == maxIterations - 1)) {
            std::cout << "Iteration " << iteration << ", error: " << error 
                      << ", lambda: " << lambda << "\n";
        }
        
        ++iteration;
    }
    
    if (verbose) {
        std::cout << "Calibration completed in " << iteration << " iterations\n";
        std::cout << "Final error: " << error << "\n";
    }
    
    return rates;
}