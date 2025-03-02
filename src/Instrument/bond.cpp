#include "bond.hpp"
#include <iostream>     // For input and output operations
#include <cmath>        // For mathematical functions like exp()
#include <algorithm>    // For algorithms like std::generate
#include <iomanip>      // For controlling the output format
#include "actual_360.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>


      Bond::Bond(const InstrumentDescription& desc)
    : issueDate(desc.issueDate), maturity(desc.maturity), couponRate(desc.couponRate),
      frequency(desc.frequency), notional(desc.notional),
      couponDates(desc.couponDates), zeroCouponCurve(desc.zeroCouponCurve) {}
    /**
     * Compute the theoretical price of the bond using discount factors.
     * @return The computed bond price.
     */
    double Bond::price() const {
    double price = 0.0;
    Actual_360 calculator;

    std::cout << "\nMaturity: " << std::setprecision(2) << maturity << " años | Cupón Rate: " 
              << std::setprecision(2) << couponRate * 100 << "% | Frecuencia: " 
              << std::setprecision(0) << frequency << " pagos por año | Notional: $" << notional << "\n";
    std::cout << std::string(95, '-') << "\n";
    std::cout << "\nCalculando Precio Teórico del Bono\n";
    std::cout << "Formula: Σ(Cupón * DF) + (Notional * DF Final)\n";
    std::cout << "\nPeriodo | Fecha Pago | Maturity | Cupón | Discount Factor | Cupón Descontado | Precio acumulado\n";
    std::cout << std::string(95, '-') << "\n";

    for (size_t i = 0; i < couponDates.size(); ++i) {
        double date = couponDates[i];
        boost::gregorian::date payment_date = issueDate + boost::gregorian::days(static_cast<int>(date * 360));
        double accrualFraction = static_cast<double>(calculator.compute_daycount(issueDate, payment_date)) / 360.0;

        double discountFactor = zeroCouponCurve->getDiscountFactor(accrualFraction);
        double coupon = (couponRate / frequency) * notional;
        double discountedCashFlow = coupon * discountFactor;

        price += discountedCashFlow;

        std::cout << std::setw(7) << (i+1) << " | "
                  << payment_date << " | "
                  << std::fixed << std::setprecision(6) << accrualFraction << " | "
                  << std::setw(6) << std::setprecision(2) << coupon << " | "
                  << std::setw(15) << std::setprecision(5) << discountFactor << " | "
                  << std::setw(16) << std::setprecision(5) << discountedCashFlow << " | "
                  << std::setw(16) << std::setprecision(5) << price << "\n";
    }

    // Flujo final: notional descontado
    boost::gregorian::date maturityDate = issueDate + boost::gregorian::days(static_cast<int>(maturity * 360));
    double finalAccrualFraction = static_cast<double>(calculator.compute_daycount(issueDate, maturityDate)) / 360.0;
    double finalDiscount = zeroCouponCurve->getDiscountFactor(finalAccrualFraction);
    double finalDiscountedPayment = notional * finalDiscount;

    price += finalDiscountedPayment;

    std::cout << std::setw(7) << "Final" << " | "
              << maturityDate << " | "
              << std::fixed << std::setprecision(6) << finalAccrualFraction << " | "
              << std::setw(6) << std::setprecision(2) << notional << " | "
              << std::setw(15) << std::setprecision(5) << finalDiscount << " | "
              << std::setw(16) << std::setprecision(5) << finalDiscountedPayment << " | "
              << std::setw(16) << std::setprecision(5) << price << "\n";

    std::cout << std::string(95, '-') << "\n";
    std::cout << "Precio Total del Bono (Suma de flujos descontados): $" << std::fixed << std::setprecision(5) << price << "\n";

    return price;
}
double Bond::yieldToMaturity(double initialGuess, int maxIterations, double tolerance, double priceBond) const {
    double estimatedYTM = initialGuess;  

    std::cout << "\nIniciando Newton-Raphson para calcular la TIR...\n";
    std::cout << "Precio del bono: " << std::setprecision(10) << priceBond << "\n";
    std::cout << "Suposición inicial: " << estimatedYTM * 100 << "%\n";
    std::cout << "Máximo de iteraciones: " << maxIterations << "\n";
    std::cout << "Tolerancia de error: " << std::scientific << tolerance << "\n\n";

    for (int iteration = 0; iteration < maxIterations; ++iteration) {
        double functionValue = 0.0;  
        double derivativeValue = 0.0; 

        double coupon = (couponRate / frequency) * notional;  
        int totalPayments = static_cast<int>(maturity * frequency);  

        // Calculador de fracción de año
        Actual_360 calculator;
        
        for (int period = 1; period <= totalPayments; ++period) {
            boost::gregorian::date payment_date = issueDate + boost::gregorian::days(static_cast<int>(period * (360.0 / frequency)));
            double accrualFraction = static_cast<double>(calculator.compute_daycount(issueDate, payment_date)) / 360.0;

            // Calcular el factor de descuento manualmente
            double discountFactor = 1.0 / pow(1.0 + estimatedYTM / frequency, accrualFraction * frequency);
            
            functionValue += coupon * discountFactor;
            derivativeValue += -accrualFraction * coupon * discountFactor / (1.0 + estimatedYTM / frequency);
        }

        // Agregar el valor nominal descontado
        double finalAccrualFraction = static_cast<double>(calculator.compute_daycount(issueDate, issueDate + boost::gregorian::days(static_cast<int>(maturity * 360)))) / 360.0;
        double finalDiscountFactor = 1.0 / pow(1.0 + estimatedYTM / frequency, finalAccrualFraction * frequency);
        
        functionValue += notional * finalDiscountFactor;
        derivativeValue += -finalAccrualFraction * notional * finalDiscountFactor / (1.0 + estimatedYTM / frequency);

        // Evaluar la diferencia con el precio de mercado
        functionValue -= priceBond;

        if (std::fabs(derivativeValue) < 1e-10) {
            std::cerr << "Error: Derivada demasiado pequeña en Newton-Raphson. Ajustando derivada...\n";
            derivativeValue = (derivativeValue < 0) ? -1e-10 : 1e-10;
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