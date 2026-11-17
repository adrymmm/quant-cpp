#include "quant/pricing/monte_carlo.hpp"
#include <algorithm>
#include <cmath>
#include <cassert>
#include <random>

namespace quant::pricing {

McResult mc_price(const Payoff& payoff, const McInputs& inputs, std::size_t n, std::mt19937_64& rng) {
    assert(n >= 2 && "mc_price: n must be >= 2 (n - 1 divisor in SE)");
    assert(inputs.S0 > 0.0 && "mc_price: S0 must be positive");
    assert(inputs.sigma > 0.0 && "mc_price: sigma must be positive");
    assert(inputs.T > 0.0 && "mc_price: T must be positive");
    
    const double m = static_cast<double>(n);
    std::normal_distribution<double> dist{0.0, 1.0};

    double sum_x = 0.0;
    double sum_x2 = 0.0;

    const double drift = (inputs.r - 0.5 * inputs.sigma * inputs.sigma) * inputs.T;
    const double vol = inputs.sigma * std::sqrt(inputs.T);
    const double discount = std::exp(-(inputs.r) * inputs.T);

    for (std::size_t i = 0; i < n; ++i) {
        const double z = dist(rng);

        const double S_T = inputs.S0 * std::exp(drift + vol * z);
        const double x = payoff(S_T);

        sum_x += x;
        sum_x2 += x * x;
    }
    double mean_x = sum_x / m;

    // clamping to 0 to avoid NaNs; can be below 0, and sqrt(<0) is NaN
    double stdev_numer = std::max(sum_x2 - m*mean_x*mean_x, 0.0);
    double stdev_den = m - 1.0;
    
    double stdev = std::sqrt(stdev_numer / stdev_den);
    double se = stdev / std::sqrt(m);
    

    return McResult{discount * mean_x, discount * se};
}

} // namespace quant::pricing
