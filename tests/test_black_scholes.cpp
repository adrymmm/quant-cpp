#include "quant/pricing/black_scholes.hpp"
#include <cmath>
#include <gtest/gtest.h>

namespace qp = quant::pricing;

namespace {

struct BsInputs { double S0, K, r, sigma, T; };
constexpr BsInputs hull{.S0 = 42.0, .K = 40.0, .r = 0.10, .sigma = 0.20, .T = 0.5};
constexpr BsInputs alt{.S0 = 100.0, .K = 110.0, .r = 0.15, .sigma = 0.30, .T = 0.9};

void run_parity_check(const BsInputs& input) {
    const double c = qp::black_scholes_call(input.S0, input.K, input.r, input.sigma, input.T);
    const double p = qp::black_scholes_put(input.S0, input.K, input.r, input.sigma, input.T);
    const double rhs = input.S0 - input.K * std::exp(-input.r * input.T);

    EXPECT_NEAR(c - p, rhs, 1e-12);
}
} // namespace


TEST(BlackScholes, CallMatchesHull) {
    EXPECT_NEAR(qp::black_scholes_call(hull.S0, hull.K, hull.r, hull.sigma, hull.T), 4.76, 0.005);
}

TEST(BlackScholes, PutMatchesHull) {
    EXPECT_NEAR(qp::black_scholes_put(hull.S0, hull.K, hull.r, hull.sigma, hull.T), 0.81, 0.005);
}

TEST(BlackScholes, PutCallParity) {
    { SCOPED_TRACE("hull"); run_parity_check(hull); }
    { SCOPED_TRACE("alt");  run_parity_check(alt); }
}

