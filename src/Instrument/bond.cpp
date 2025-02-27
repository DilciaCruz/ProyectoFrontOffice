#include "bond.hpp"
#include <iostream>     // For input and output operations
#include <cmath>        // For mathematical functions like exp()
#include <algorithm>    // For algorithms like std::generate
#include <iomanip>      // For controlling the output format
#include "actual_360.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>

Bond::Bond(boost::gregorian::date issueDate, double maturity, double couponRate, 
           double frequency, double notional, const std::vector<double>& couponDates, 
           const std::shared_ptr<ZeroCouponCurve>& curve)
    : issueDate(issueDate), maturity(maturity), couponRate(couponRate), 
      frequency(frequency), notional(notional), couponDates(couponDates), zeroCouponCurve(curve) {}
    /**
     * Compute the theoretical price of the bond using discount factors.
     * @return The computed bond price.
     */
double Bond::price() const{
        double price = 0.0;
        Actual_360 calculator;

        std::cout << "\nMaturity: " << std::setprecision(2) << maturity << " años | Cupón Rate: " << std::setprecision(2) << couponRate * 100 << "% | Frecuencia: " << std::setprecision(0) <<frequency 
                  << " pays/year | Notional: $" << notional << "\n";
        std::cout << std::string(65, '-') << "\n";
        std::cout << "\nCalculate Bond Price"<<std::endl ;
        std::cout << "Formula used to calculate Bond Price Σ(Coupon * e^(-rate * matutity)): being Discount Factor = e^(-rate * matutity) " << "\n";
        std::cout << "\nCoupon | Maturity | Discount Factor | Present Cash Flows\n";
        std::cout << std::string(65, '-') << "\n";

        for (double date : couponDates) {
            boost::gregorian::date payment_date = issueDate + boost::gregorian::days(static_cast<int>(date * 360));
            double accrualFraction = static_cast<double>(calculator.compute_daycount(issueDate, payment_date)) / 360.0;

            double discountFactor = zeroCouponCurve->getDiscountFactor(accrualFraction);
            double coupon = (couponRate / frequency) * notional;
            price += coupon * discountFactor;
            std::cout << std::setw(6) << std::setprecision(0) << std::fixed << coupon
                      << " | " << std::setw(8) << std::setprecision(2) << std::fixed << accrualFraction
                      << " | " << std::setw(15) << std::setprecision(5) << discountFactor
                      << " | " << std::setw(15) << std::setprecision(4) << price << "\n";
                        
     
        }
        // Add discounted notional amount
        boost::gregorian::date maturityDate = issueDate + boost::gregorian::days(static_cast<int>(maturity * 360));
        double finalAccrualFraction = static_cast<double>(calculator.compute_daycount(issueDate, maturityDate)) / 360.0;
        double finalDiscount = zeroCouponCurve->getDiscountFactor(finalAccrualFraction);
        price += notional * finalDiscount;
        std::cout << std::setw(35) << std::setprecision(5) << finalDiscount << " |" << std::setw(16) << std::setprecision(2) << price << "\n";
        std::cout << std::string(65, '-') << "\n";
        return price;
    }
double Bond::yieldToMaturity(double initialGuess, int maxIterations, double tolerance, double priceBond) const {
    double estimatedYTM = initialGuess;  // Suposición inicial de la TIR

    std::cout << "\nIniciando Newton-Raphson para calcular la TIR...\n";
    std::cout << "Precio del bono: " << std::setprecision(10) << priceBond << "\n";
    std::cout << "Suposición inicial: " << estimatedYTM * 100 << "%\n";
    std::cout << "Máximo de iteraciones: " << maxIterations << "\n";
    std::cout << "Tolerancia de error: " << std::scientific << tolerance << "\n\n";

    for (int iteration = 0; iteration < maxIterations; ++iteration) {
        double functionValue = 0.0;  // Representa f(YTM)
        double derivativeValue = 0.0; // Representa f'(YTM)

        double coupon = (couponRate / frequency) * notional;  // Pago del cupón por período
        int totalPayments = static_cast<int>(maturity * frequency);  // Total de pagos en la vida del bono

        // Definir el factor de descuento base
        double discountFactorBase = 1.0 + estimatedYTM / frequency;

        if (discountFactorBase <= 0) {
            std::cerr << "Error: División por cero en Newton-Raphson." << std::endl;
            return estimatedYTM;
        }

        // Calculador de fracción de año
        Actual_360 calculator;
        
        for (int period = 1; period <= totalPayments; ++period) {
            boost::gregorian::date payment_date = issueDate + boost::gregorian::days(static_cast<int>(period * (360.0 / frequency)));
            double accrualFraction = static_cast<double>(calculator.compute_daycount(issueDate, payment_date)) / 360.0;

            double discountFactor = std::pow(discountFactorBase, -accrualFraction);  // Se usa signo negativo aquí
            functionValue += coupon * discountFactor;
            derivativeValue += -accrualFraction * coupon * discountFactor / discountFactorBase;
        }

        // Agregar el valor nominal descontado
        double finalAccrualFraction = static_cast<double>(calculator.compute_daycount(issueDate, issueDate + boost::gregorian::days(static_cast<int>(maturity * 360)))) / 360.0;
        double finalDiscountFactor = std::pow(discountFactorBase, -finalAccrualFraction);
        functionValue += notional * finalDiscountFactor;
        derivativeValue += -finalAccrualFraction * notional * finalDiscountFactor / discountFactorBase;

        // Evaluar la diferencia con el precio de mercado
        functionValue -= priceBond;

        if (std::fabs(derivativeValue) < 1e-10) {
            std::cerr << "Error: Derivada demasiado pequeña en Newton-Raphson." << std::endl;
            return estimatedYTM;
        }

        // Aplicamos Newton-Raphson
        double newEstimatedYTM = estimatedYTM - functionValue / derivativeValue;
        double error = std::fabs(newEstimatedYTM - estimatedYTM);

        std::cout << "Iteración " << iteration + 1 
                  << " | YTM: " << std::setprecision(6) << newEstimatedYTM * 100 << "% | Error: " 
                  << std::scientific << error << "\n";

        if (error < tolerance) {
            std::cout << "\n Convergencia alcanzada en " << iteration + 1 
                      << " iteraciones con error: " << error << "\n";
            return newEstimatedYTM;
        }

        estimatedYTM = newEstimatedYTM;
    }

    std::cerr << "\n No se encontró la TIR dentro del límite de " << maxIterations 
              << " iteraciones. Último YTM calculado: " << estimatedYTM * 100 << "%\n";
    return estimatedYTM;
}