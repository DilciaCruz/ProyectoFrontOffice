#include "bond.hpp"
#include <iostream>     // For input and output operations
#include <cmath>        // For mathematical functions like exp()
#include <algorithm>    // For algorithms like std::generate
#include <iomanip>      // For controlling the output format

Bond::Bond(double maturity, double couponRate, double frequency, double notional, const std::vector<double>& couponDates, const std::shared_ptr<YieldCurve>&curve)
    : maturity(maturity), couponRate(couponRate), frequency(frequency), notional(notional),
    couponDates(couponDates), yieldCurve(curve){}
    /**
     * Compute the theoretical price of the bond using discount factors.
     * @return The computed bond price.
     */
double Bond::price() const{
        double price = 0.0;
        std::cout << "\nMaturity: " << std::setprecision(2) << maturity << " años | Cupón Rate: " << std::setprecision(2) << couponRate * 100 << "% | Frecuencia: " << std::setprecision(0) <<frequency 
                  << " pays/year | Notional: $" << notional << "\n";
        std::cout << std::string(65, '-') << "\n";
        std::cout << "\nCalculate Bond Pice"<<std::endl ;
        std::cout << "Formula used to calculate Bond Price Σ(Coupon * e^(-rate * matutity)): being Discount Factor = e^(-rate * matutity) " << "\n";
        std::cout << "\nCoupon | Maturity | Discount Factor | Present Cash Flows\n";
        std::cout << std::string(65, '-') << "\n";

        for (double date : couponDates) {
            double discountFactor = yieldCurve->getZero(date); // Get discount factor for each date
            double coupon = (couponRate / frequency) * notional;
            price += coupon * discountFactor;
            std::cout << std::setw(6) << std::setprecision(0) << std::fixed << coupon
                      << " | " << std::setw(8) << std::setprecision(2) << std::fixed << date
                      << " | " << std::setw(15) << std::setprecision(5) << discountFactor
                      << " | " << std::setw(15) << std::setprecision(4) << price << "\n";
                        
     
        }
        // Add discounted notional amount
        double finalDiscount = yieldCurve->getZero(maturity);
        price += notional * finalDiscount;
        std::cout << std::setw(35) << std::setprecision(5) << finalDiscount << " |" << std::setw(16) << std::setprecision(4) << price << "\n";
        std::cout << std::string(65, '-') << "\n";
        return price;
    }

double Bond::yieldToMaturity(double initialGuess, int maxIterations, double tolerance) const {
    double estimatedYTM = initialGuess;  // Suposición inicial de la TIR
    double bondPrice = price();  // Precio del bono calculado

    std::cout << "\nIniciando Newton-Raphson para calcular la TIR...\n";
    std::cout << "Suposición inicial: " << estimatedYTM * 100 << "%\n";
    std::cout << "Máximo de iteraciones: " << maxIterations << "\n";
    std::cout << "Tolerancia de error: " << tolerance << "\n\n";
    
    for (int iteration = 0; iteration < maxIterations; ++iteration) {
        double functionValue = 0.0;  // Representa f(YTM)
        double derivativeValue = 0.0; // Representa f'(YTM)

        double coupon = (couponRate / frequency) * notional;  // Pago del cupón por período
        int totalPayments = static_cast<int>(maturity * frequency);  // Total de pagos en la vida del bono

        // Definir el factor de descuento
        double discountFactorBase = 1 + estimatedYTM / frequency;

        // Calculamos f(YTM) y f'(YTM)
        for (int period = 1; period <= totalPayments; ++period) {
            double discountFactor = std::pow(discountFactorBase, period); // (1 + YTM / frequency)^period
            functionValue += coupon / discountFactor;  // Suma de los flujos descontados
            derivativeValue += -period * coupon / (discountFactor * discountFactorBase);
        }

        // Agregar el valor nominal descontado
        double finalDiscountFactor = std::pow(discountFactorBase, totalPayments);
        functionValue += notional / finalDiscountFactor;
        derivativeValue += -totalPayments * notional / (finalDiscountFactor * discountFactorBase);

        // Evaluar la diferencia con el precio de mercado
        functionValue -= bondPrice;

        // Aplicamos Newton-Raphson
        double newEstimatedYTM = estimatedYTM - functionValue / derivativeValue;
        double error = std::fabs(newEstimatedYTM - estimatedYTM);

        // Imprimir progreso de iteración
        std::cout << "Iteración " << iteration + 1 
                  << " | YTM: " << newEstimatedYTM * 100 << "% | Error: " << error << "\n";

        // Verificamos convergencia
        if (error < tolerance) {
            std::cout << "\nConvergencia alcanzada en " << iteration + 1 
                      << " iteraciones con error: " << error << "\n";
            return newEstimatedYTM;
        }

        estimatedYTM = newEstimatedYTM;
    }

    std::cerr << "\nNo se encontró la TIR dentro del límite de " << maxIterations 
              << " iteraciones. Último YTM calculado: " << estimatedYTM * 100 << "%\n";
    return estimatedYTM;
}
 