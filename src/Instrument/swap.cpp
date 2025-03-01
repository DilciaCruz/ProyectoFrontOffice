
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

    std::cout << "\n>>> Calculando precio del swap:\n";
    std::cout << "Notional: " << notional_ << "\n";
    std::cout << "Fixed Rate: " << fixedRate_ << "\n";
    std::cout << "Maturity: " << maturity_ << " años\n";
    std::cout << "Fixed Frequency: " << fixedFrequency_ << "\n";
    std::cout << "Floating Frequency: " << floatingFrequency_ << "\n\n";

    boost::gregorian::date currentDate = issueDate_;
    double previousTime = 0.0;
    double currentFloatingRate = initialFloatingRate_;

    for (double t = fixedFrequency_; t <= maturity_; t += fixedFrequency_) {
        boost::gregorian::date nextPaymentDate = currentDate + boost::gregorian::months(static_cast<int>(12 / fixedFrequency_));

        double accrual = dayCountCalculator->compute_daycount(currentDate, nextPaymentDate) / 360.0;
        double DF = zeroCouponCurve_->getDiscountFactor(t);

        std::cout << "[Periodo t = " << t 
                  << "] Fecha: " << boost::gregorian::to_simple_string(nextPaymentDate) 
                  << " | Accrual: " << accrual 
                  << " | Discount Factor: " << DF << "\n";

        double fixedCashFlow = notional_ * fixedRate_ * accrual;
        fixedLegPV += fixedCashFlow * DF;

        std::cout << "  -> Fixed Cash Flow: " << fixedCashFlow << " | PV Fixed: " << fixedLegPV << "\n";

        if (t > fixedFrequency_) {
            double forwardRate = zeroCouponCurve_->forwardRate(previousTime, t);
            currentFloatingRate = zeroCouponCurve_->continuousToEffective(forwardRate, floatingFrequency_);
        }

        double floatingCashFlow = notional_ * currentFloatingRate * accrual;
        floatingLegPV += floatingCashFlow * DF;

        std::cout << "  -> Floating Rate: " << currentFloatingRate 
                  << " | Floating Cash Flow: " << floatingCashFlow 
                  << " | PV Floating: " << floatingLegPV << "\n\n";

        previousTime = t;
        currentDate = nextPaymentDate;  // Avanza la fecha base para el próximo pago
    }

    double finalDF = zeroCouponCurve_->getDiscountFactor(maturity_);
    fixedLegPV += notional_ * finalDF;
    floatingLegPV += notional_ * finalDF;

    std::cout << "Valor Nominal Final Descontado (DF=" << finalDF << "): " 
              << notional_ * finalDF << "\n\n";

    double swapPrice = floatingLegPV - fixedLegPV;

    std::cout << ">>> Precio del Swap calculado: " << swapPrice << "\n";

    return swapPrice;
}