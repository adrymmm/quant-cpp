#pragma once
#include "quant/pricing/payoff.hpp"
#include <random> 
#include <cstddef>

namespace quant::pricing {
// Market inputs. S0, sigma, T need to be positive; r unrestricted
struct McInputs { double S0, r, sigma, T; };

// `price`: discounted MC estimate (to today), `std_error`: std error of price
struct McResult {
    double price;
    double std_error;
};

// Prices `payoff` by Monte Carlo over `n` paths under GBM.
// Requires n >= 2 (asserted). Advances `rng` by n draws.
McResult mc_price(const Payoff& payoff, const McInputs& inputs, std::size_t n, std::mt19937_64& rng); 
} // namespace quant::pricing

