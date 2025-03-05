#define BOOST_TEST_MODULE DiscountCurveCalibrationTest
#include <boost/test/unit_test.hpp>
#include "../discount_curve_calibration.hpp"
#include "../bond.hpp"
#include "../bond_builder.hpp"
#include "../swap.hpp"
#include "../swap_builder.hpp"
#include "../factory_registrator.hpp"
#include "../instrument_description.hpp"
#include "../factory.hpp"
#include <iostream>
#include <iomanip>

// Register the builders automatically
static FactoryRegistrator<BondBuilder> bondRegistrator;
static FactoryRegistrator<SwapBuilder> swapRegistrator;

BOOST_AUTO_TEST_SUITE(DiscountCurveCalibrationSuite)

// Helper function to create a bond description
InstrumentDescription createBondDescription(double maturity, double coupon,
                                          const boost::gregorian::date& issueDate) {
    InstrumentDescription bondDesc(InstrumentDescription::bond);
    bondDesc.maturity = maturity;
    bondDesc.couponRate = coupon;
    bondDesc.frequency = 2.0;     // Semi-annual
    bondDesc.notional = 100.0;
    bondDesc.issueDate = issueDate;
    
    // Generate coupon dates
    std::vector<double> couponDates;
    for (double t = 0.5; t <= maturity; t += 0.5) {
        couponDates.push_back(t);
    }
    bondDesc.couponDates = couponDates;
    
    return bondDesc;
}

// Helper function to create a swap description
InstrumentDescription createSwapDescription(double maturity, double fixedRate,
                                          const boost::gregorian::date& tradeDate) {
    InstrumentDescription swapDesc(InstrumentDescription::swap);
    swapDesc.maturity = maturity;
    swapDesc.fixedRate = fixedRate;  
    swapDesc.fixedFrequency = 2.0;   
    swapDesc.floatingFrequency = 2.0;
    swapDesc.notional = 100.0;
    swapDesc.issueDate = tradeDate;
    
    // ADDED: Required parameters for swaps
    swapDesc.floatingIndex = "LIBOR6M";  
    swapDesc.initialFixing = 0.05;       
    swapDesc.dayCountConvention = "ACT/360"; 
    
    // Generate payment dates for both legs
    std::vector<double> couponDates;  // FIXED: changed paymentDates to couponDates
    for (double t = 0.5; t <= maturity + 0.001; t += 0.5) {
        couponDates.push_back(t);
    }
    swapDesc.couponDates = couponDates;  // FIXED: changed paymentDates to couponDates
    
    return swapDesc;
}

// Helper function to create a deposit description
InstrumentDescription createDepositDescription(double maturity, double rate,
                                             const boost::gregorian::date& startDate) {
    // For deposits, we'll use a zero-coupon bond
    InstrumentDescription depositDesc(InstrumentDescription::bond);
    depositDesc.maturity = maturity;
    depositDesc.couponRate = 0.0;  // Zero coupon
    depositDesc.frequency = 1.0;   
    depositDesc.notional = 100.0;
    depositDesc.issueDate = startDate;
    depositDesc.couponDates = {maturity}; // Only payment at maturity
    
    // For a deposit with rate r and maturity T, price is 100/(1+r*T)
    // But we'll let the calibration handle this
    
    return depositDesc;  // CORRECTED: proper function completion
}

// Keep your existing test case
BOOST_AUTO_TEST_CASE(TestDiscountCurveCalibration) {
    // ... existing test code remains unchanged ...
}

// Add the new test case with the specified market data
BOOST_AUTO_TEST_CASE(TestMarketDataCalibration) {
    BOOST_TEST_MESSAGE("Testing discount curve calibration with April 2016 market data");
    
    // Valuation date: April 1, 2016
    boost::gregorian::date valuationDate(2016, 4, 1);
    
    // Create calibration object
    DiscountCurveCalibration calibration(valuationDate);
    
    // Market data:
    // - Depósito 6m r = 5%
    // - Swap 12m r = 5.5%
    // - Swap 18m r = 6%
    // - Swap 2y r = 6.4%
    
    // Add 6-month deposit
    InstrumentDescription deposit = createDepositDescription(0.5, 0.05, valuationDate);
    double depositPrice = 100.0 / (1.0 + 0.05 * 0.5); // Simple interest formula
    calibration.addInstrument(deposit, depositPrice);
    
    // Add swaps - for par swaps, the price at inception should be 100
    InstrumentDescription swap1y = createSwapDescription(1.0, 0.055, valuationDate);
    InstrumentDescription swap18m = createSwapDescription(1.5, 0.06, valuationDate);
    InstrumentDescription swap2y = createSwapDescription(2.0, 0.064, valuationDate);
    
    calibration.addInstrument(swap1y, 100.0);
    calibration.addInstrument(swap18m, 100.0);
    calibration.addInstrument(swap2y, 100.0);
    
    // Initial guesses for rates at the knot points (in percentage form)
    std::vector<double> maturities = {0.5, 1.0, 1.5, 2.0};
    std::vector<double> initialGuess = {0.05, 0.055, 0.06, 0.064};
    std::vector<double> knots = maturities;
    
    // Run calibration
    std::shared_ptr<ZeroCouponCurve> calibratedCurve = calibration.calibrate(
        initialGuess, knots, 1e-6, 100, true
    );
    
    // Check that we have a valid curve
    BOOST_REQUIRE(calibratedCurve != nullptr);
    
    // Check calibration error is within reasonable bounds
    double totalError = calibration.getTotalError();
    BOOST_TEST_MESSAGE("Calibration total error: " << totalError);
    BOOST_CHECK(totalError < 0.01); // Error should be less than 1%
    
    // Print the calibrated zero rates and discount factors
    BOOST_TEST_MESSAGE("Calibrated curve points:");
    BOOST_TEST_MESSAGE("Maturity (years) | Zero Rate (%) | Discount Factor");
    BOOST_TEST_MESSAGE("------------------------------------------------");
    
    for (size_t i = 0; i < knots.size(); ++i) {
        double df = calibratedCurve->getDiscountFactor(knots[i]);
        // FIXED: Calculate zero rate from discount factor
        double zeroRate = -std::log(df) / knots[i]; 
        
        BOOST_TEST_MESSAGE(std::fixed << std::setprecision(2) << std::setw(15) << knots[i] << " | " 
                          << std::setprecision(4) << std::setw(12) << zeroRate * 100.0 << " | " 
                          << std::setw(14) << df);
        
        // Discount factors should be decreasing with time
        if (i > 0) {
            double prevDf = calibratedCurve->getDiscountFactor(knots[i-1]);
            BOOST_CHECK(df < prevDf);
        }
    }
    
    // Test the calibrated curve by pricing the instruments again
    // For this, we need to set the calibrated curve on each instrument
    deposit.zeroCouponCurve = calibratedCurve;
    swap1y.zeroCouponCurve = calibratedCurve;
    swap18m.zeroCouponCurve = calibratedCurve;
    swap2y.zeroCouponCurve = calibratedCurve;
    
    // Create the instruments and price them
    std::unique_ptr<Instrument> depositInst = Factory::instance()(deposit);
    std::unique_ptr<Instrument> swap1yInst = Factory::instance()(swap1y);
    std::unique_ptr<Instrument> swap18mInst = Factory::instance()(swap18m);
    std::unique_ptr<Instrument> swap2yInst = Factory::instance()(swap2y);
    
    double depositModelPrice = depositInst->price();
    double swap1yModelPrice = swap1yInst->price();
    double swap18mModelPrice = swap18mInst->price();
    double swap2yModelPrice = swap2yInst->price();
    
    // Check that the model prices are close to the market prices
    BOOST_TEST_MESSAGE("\nModel vs Market prices:");
    BOOST_TEST_MESSAGE("Instrument | Market Price | Model Price | Diff");
    BOOST_TEST_MESSAGE("--------------------------------------------");
    BOOST_TEST_MESSAGE("Deposit 6m |" << std::setw(13) << depositPrice << " |" 
                     << std::setw(12) << depositModelPrice << " |" 
                     << std::setw(6) << depositModelPrice - depositPrice);
    BOOST_TEST_MESSAGE("Swap 1y    |" << std::setw(13) << 100.0 << " |" 
                     << std::setw(12) << swap1yModelPrice << " |" 
                     << std::setw(6) << swap1yModelPrice - 100.0);
    BOOST_TEST_MESSAGE("Swap 18m   |" << std::setw(13) << 100.0 << " |" 
                     << std::setw(12) << swap18mModelPrice << " |" 
                     << std::setw(6) << swap18mModelPrice - 100.0);
    BOOST_TEST_MESSAGE("Swap 2y    |" << std::setw(13) << 100.0 << " |" 
                     << std::setw(12) << swap2yModelPrice << " |" 
                     << std::setw(6) << swap2yModelPrice - 100.0);
    
    BOOST_CHECK(std::abs(depositModelPrice - depositPrice) < 0.1);
    BOOST_CHECK(std::abs(swap1yModelPrice - 100.0) < 0.1);
    BOOST_CHECK(std::abs(swap18mModelPrice - 100.0) < 0.1);
    BOOST_CHECK(std::abs(swap2yModelPrice - 100.0) < 0.1);
    
    // Test interpolation at a non-knot point
    double interpMaturity = 0.75; // 9 months
    double interpDF = calibratedCurve->getDiscountFactor(interpMaturity);
    // FIXED: Calculate interpolated zero rate from discount factor
    double interpZeroRate = -std::log(interpDF) / interpMaturity;
    
    BOOST_TEST_MESSAGE("\nInterpolated at " << interpMaturity << " years:");
    BOOST_TEST_MESSAGE("Zero Rate: " << interpZeroRate * 100.0 << "%, Discount Factor: " << interpDF);
    
    // The 9-month point should be between 6m and 1y values
    BOOST_CHECK(interpDF < calibratedCurve->getDiscountFactor(0.5));
    BOOST_CHECK(interpDF > calibratedCurve->getDiscountFactor(1.0));
}

BOOST_AUTO_TEST_SUITE_END()