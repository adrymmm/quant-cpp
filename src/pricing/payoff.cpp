#include "quant/pricing/payoff.hpp"
#include <algorithm>
#include <cassert>

namespace quant::pricing {

    CallPayoff::CallPayoff(double strike) : K_{strike} {
        assert(K_ >= 0 && "pricing:CallPayoff: Strike Price K needs to be larger or equal to 0");
                }
    double CallPayoff::operator()(double S) const {
        return std::max(S - K_, 0.0);
    }

    PutPayoff::PutPayoff(double strike) : K_{strike} {
        assert(K_ >= 0 && "pricing:PutPayoff: Strike Price K needs to be larger or equal to 0");
                }
    double PutPayoff::operator()(double S) const {
        return std::max(K_ - S, 0.0);
    }

}

