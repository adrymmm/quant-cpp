#pragma once
namespace quant::pricing {
    
double black_scholes_call(double S0, double K, double r, double sigma, double T);
double black_scholes_put(double S0, double K, double r, double sigma, double T);

} // namespace quant::pricing
