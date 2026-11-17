#include "quant/pricing/payoff.hpp"
#include <gtest/gtest.h>

namespace qp = quant::pricing;

namespace {

double evalThroughInterface(const qp::Payoff& payoff, double S) {
    return payoff(S);
}

} //namespace

TEST(PayoffTest, DispatchesThroughBase) {
    const qp::CallPayoff call{30.0};
    double pc = evalThroughInterface(call, 50.0);
    EXPECT_DOUBLE_EQ(pc, 20.0);

    const qp::PutPayoff put{30.0};
    double pp = evalThroughInterface(put, 50.0);
    EXPECT_DOUBLE_EQ(pp, 0.0);
}

TEST(PayoffTest, InTheMoneyPays) {
    // Set strike
    const qp::CallPayoff call{40.0};
    // evaluate spot
    double pc = call(60.0);
    EXPECT_DOUBLE_EQ(pc, 20.0);
    
    // Set strike
    const qp::PutPayoff put{50.0};
    // evaluate spot
    double pp = put(20.0);
    EXPECT_DOUBLE_EQ(pp, 30.0);
} 


TEST(PayoffTest, OutTheMoneyZero) {
    // Set strike
    const qp::CallPayoff call{40.0};
    // evaluate spot
    double pc = call(20.0);
    EXPECT_DOUBLE_EQ(pc, 0.0);
    
    // Set strike
    const qp::PutPayoff put{50.0};
    // evaluate spot
    double pp = put(60.0);
    EXPECT_DOUBLE_EQ(pp, 0.0);
} 

TEST(PayoffTest, AtMoneyZero) {
    // Set strike
    const qp::CallPayoff call{40.0};
    // evaluate spot
    double pc = call(40.0);
    EXPECT_DOUBLE_EQ(pc, 0.0);
    
    // Set strike
    const qp::PutPayoff put{50.0};
    // evaluate spot
    double pp = put(50.0);
    EXPECT_DOUBLE_EQ(pp, 0.0);
}
