#include "swap.hpp"
#include "actual_360.hpp"
#include "thirty_360.hpp"
#include "day_count_calculator.hpp"
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
    std::cout << "\n>>> Calculando flujos y precio del swap:\n";
    std::cout << "Notional: " << notional_ << "\n"
              << "Fixed Rate: " << fixedRate_ << "\n"
              << "Maturity: " << maturity_ << " años\n"
              << "Fixed Frequency: " << fixedFrequency_ << " pagos por año\n"
              << "Floating Frequency: " << floatingFrequency_ << " pagos por año\n\n";

    std::unique_ptr<DayCountCalculator> dayCountCalculator;
    if (dayCountConvention_ == "ACT/360") {
        dayCountCalculator = std::make_unique<Actual_360>();
    } else if (dayCountConvention_ == "30/360") {
        dayCountCalculator = std::make_unique<Thirty_360>();
    } else {
        throw std::invalid_argument("Convención de días no soportada.");
    }

    boost::gregorian::date paymentDate = issueDate_;
    boost::gregorian::date previousPaymentDate = issueDate_;

    double previousTime = 0.0;
    double currentFloatingRate = initialFloatingRate_;

    double pvFixed = 0.0;
    double pvFloating = 0.0;

    std::cout << "Periodo | Fecha Pago | dcf Flotante | dcf FIjo | DF | Forward Rate | Float Rate | Fixed CF | Float CF | PV Fixed | PV Float\n";
    std::cout << "----------------------------------------------------------------------------------------------------------------------------------------\n";

    for (int period = 1; period <= static_cast<int>(maturity_ * fixedFrequency_); ++period) {
        paymentDate += boost::gregorian::months(static_cast<int>(12 / fixedFrequency_));

        double timeToPayment = static_cast<double>(dayCountCalculator->compute_daycount(issueDate_, paymentDate)) / 360.0;
        double accrual = static_cast<double>(dayCountCalculator->compute_daycount(previousPaymentDate, paymentDate)) / 360.0;

        double DF = zeroCouponCurve_->getDiscountFactor(timeToPayment);

        double forwardContinuous = 0.0;
        if (period > 1) {
            forwardContinuous = zeroCouponCurve_->forwardRate(previousTime, timeToPayment);
            currentFloatingRate = fixedFrequency_ * (std::exp(forwardContinuous / fixedFrequency_) - 1);
        }

        double fixedCashFlow = notional_ * fixedRate_ * accrual;
        double floatingCashFlow = notional_ * currentFloatingRate * accrual;

        pvFixed += fixedCashFlow * DF;
        pvFloating += floatingCashFlow * DF;

        std::cout << std::setw(7) << period << " | "
                << paymentDate << " | "
                << std::fixed << std::setprecision(6) << timeToPayment << " | "
                << std::fixed << std::setprecision(6) << accrual << " | "
                << std::fixed << std::setprecision(5) << DF << " | "
                << std::fixed << std::setprecision(6) << forwardContinuous << " | "
                << std::fixed << std::setprecision(3) << (currentFloatingRate * 100) << "% | "
                << std::setw(8) << std::setprecision(3) << fixedCashFlow << "M | "
                << std::setw(8) << std::setprecision(3) << floatingCashFlow << "M | "
                << std::setw(8) << std::setprecision(3) << pvFixed << "M | "
                << std::setw(8) << std::setprecision(3) << pvFloating << "M\n";

        previousPaymentDate = paymentDate;
        previousTime = timeToPayment;
    }
    double finalDF = zeroCouponCurve_->getDiscountFactor(maturity_);
    pvFixed += notional_ * finalDF;
    pvFloating += notional_ * finalDF;

    std::cout << "---------------------------------------------------------------------------------------------------\n";
    std::cout << "Valor Presente al | " << paymentDate << " Con un Factor de descuento de: " 
              << std::fixed << std::setprecision(5) << finalDF << " "
              << " y un valor presente fijo de: " 
              << pvFixed << "M | "
              << " y un valor presente flotante de: " 
              << pvFloating << "M\n";

    double npv = pvFixed - pvFloating;

    std::cout << "\n>>> Precio calculado del Swap (NPV): " << npv << "\n";

    return npv;
}