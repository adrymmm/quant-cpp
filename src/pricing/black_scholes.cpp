#include "quant/pricing/black_scholes.hpp"
#include <cmath>
#include <numbers>
#include <cassert>

namespace {

struct D1D2 {
    double d1;
    double d2;
};

double norm_CDF(double value) {
   return 0.5 * std::erfc(-value * (1.0 / std::numbers::sqrt2));
}

D1D2 calc_d1_d2(double S0, double K, double r, double sigma, double T) {
    assert(S0 > 0.0 && "S0 needs to be positive ");
    assert(K > 0.0 && "K needs to be positive ");
    assert(sigma > 0.0 && "sigma needs to be positive ");
    assert(T > 0.0 && "T needs to be positive ");

    const double vol_sqrt_t = sigma * std::sqrt(T);
    const double d1 = (std::log(S0 / K) + ((r + ((sigma * sigma ) / 2)) * T) ) / vol_sqrt_t;
    const double d2 = d1 - vol_sqrt_t;

    return D1D2{d1, d2};
}

} // namespace

namespace quant::pricing {

double black_scholes_call(double S0, double K, double r, double sigma, double T) {
    const auto [d1, d2] = calc_d1_d2(S0, K, r, sigma, T);
    return S0 * norm_CDF(d1) - K * std::exp(-r * T) * norm_CDF(d2);
}

double black_scholes_put(double S0, double K, double r, double sigma, double T) {
    const auto [d1, d2] = calc_d1_d2(S0, K, r, sigma, T);
    return K * std::exp(-r * T) * norm_CDF(-d2) - S0 * norm_CDF(-d1);
}

} // namespace quant::pricing
