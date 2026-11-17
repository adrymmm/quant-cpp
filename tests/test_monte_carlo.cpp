#include "quant/pricing/monte_carlo.hpp"
#include "quant/pricing/payoff.hpp"
#include "quant/pricing/black_scholes.hpp"
#include <cmath>
#include <random>
#include <gtest/gtest.h>

namespace qp = quant::pricing;

namespace {
// same hull inputs as test_black_scholes.cpp
constexpr qp::McInputs hull_mc{.S0 = 42.0, .r = 0.10, .sigma = 0.20, .T = 0.5};
constexpr double hull_K = 40.0;

} // namespace

TEST(MonteCarlo, CallThreeSEBlackScholes) {
    std::mt19937_64 rng{42};
    const qp::CallPayoff call{hull_K};
    
    const qp::McResult mc_res = qp::mc_price(call, hull_mc, 100'000, rng);
    const double c_bs = qp::black_scholes_call(hull_mc.S0, hull_K, hull_mc.r, hull_mc.sigma, hull_mc.T);
    
    EXPECT_LT(std::abs(mc_res.price - c_bs), 3.0 * mc_res.std_error) << "mc=" << mc_res.price 
        << " bs=" << c_bs  << " se=" << mc_res.std_error;
}
