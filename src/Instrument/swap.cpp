#include "swap.hpp"
#include "actual_360.hpp"
#include "thirty_360.hpp"
#include <iostream>
#include <iomanip>
#include <cmath>

Swap::Swap(const InstrumentDescription& desc)
    : notional_(desc.notional), fixedRate_(desc.fixedRate),
      fixedFrequency_(desc.fixedFrequency), floatingFrequency_(desc.floatingFrequency),
      initialFloatingRate_(desc.initialFixing), floatingIndex_(desc.floatingIndex),
      dayCountConvention_(desc.dayCountConvention), issueDate_(desc.issueDate),
      maturity_(desc.maturity), zeroCouponCurve_(desc.zeroCouponCurve) {}

double Swap::price() const {
    std::cout << "\n>>> Calculando precio del swap:\n";
    std::cout << "Notional: " << notional_ << "\n"
              << "Fixed Rate: " << fixedRate_ << "\n"
              << "Maturity: " << maturity_ << " años\n"
              << "Fixed Frequency: " << fixedFrequency_ << "\n"
              << "Floating Frequency: " << floatingFrequency_ << "\n\n";

    double fixedLegPV = 0.0;
    double floatingLegPV = 0.0;

    std::unique_ptr<DayCountCalculator> dayCountCalculator;
    if (dayCountConvention_ == "ACT/360") {
        dayCountCalculator = std::make_unique<Actual_360>();
    } else if (dayCountConvention_ == "30/360") {
        dayCountCalculator = std::make_unique<Thirty_360>();
    } else {
        throw std::invalid_argument("Convención de días no soportada.");
    }

    boost::gregorian::date paymentDate = issueDate_;
    double previousTime = 0.0;
    double currentFloatingRate = initialFloatingRate_;

    std::cout << "Periodo | Fecha Pago | Accrual | DF | Fixed CF | PV Fixed | Float Rate | Float CF | PV Float\n";
    std::cout << "------------------------------------------------------------------------------------------------\n";

    for (int period = 1; paymentDate < issueDate_ + boost::gregorian::years(static_cast<int>(maturity_)); ++period) {
        paymentDate += boost::gregorian::months(static_cast<int>(12 / fixedFrequency_));
        double t = period * (1.0 / fixedFrequency_);
        double accrual = dayCountCalculator->compute_daycount(issueDate_, paymentDate) / 360.0;
        double DF = zeroCouponCurve_->getDiscountFactor(t);

        double fixedCashFlow = notional_ * fixedRate_ * accrual;
        double pvFixed = fixedCashFlow * DF;
        fixedLegPV += pvFixed;

        // Forward y efectivo solo después del primer periodo
        if (period > 1) {
            double forwardRate = zeroCouponCurve_->forwardRate(previousTime, t);
            currentFloatingRate = zeroCouponCurve_->continuousToEffective(forwardRate, floatingFrequency_);
        }

        double floatingCashFlow = notional_ * currentFloatingRate * accrual;
        double pvFloating = floatingCashFlow * DF;
        floatingLegPV += pvFloating;

        std::cout << std::setw(7) << period << " | "
                  << paymentDate << " | "
                  << std::fixed << std::setprecision(6) << accrual << " | "
                  << std::fixed << std::setprecision(5) << DF << " | "
                  << std::scientific << fixedCashFlow << " | "
                  << pvFixed << " | "
                  << std::fixed << std::setprecision(3) << (currentFloatingRate * 100) << "% | "
                  << std::scientific << floatingCashFlow << " | "
                  << pvFloating << "\n";

        previousTime = t;
    }

    double finalDF = zeroCouponCurve_->getDiscountFactor(maturity_);
    fixedLegPV += notional_ * finalDF;
    floatingLegPV += notional_ * finalDF;

    std::cout << "------------------------------------------------------------------------------------------------\n";
    std::cout << "Final   | " << paymentDate << " |    -    | " 
              << std::fixed << std::setprecision(5) << finalDF << " | "
              << "        - | " << notional_ * finalDF << " |"
              << "        - |         - | " << notional_ * finalDF << "\n";

    double price = floatingLegPV - fixedLegPV;
    std::cout << "\n>>> Precio calculado del Swap: " << price << "\n\n";

    return price;
}