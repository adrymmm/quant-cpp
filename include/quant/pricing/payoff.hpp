#pragma once

namespace quant::pricing {

// Payoff of a European option
// operator()(S): S is terminal spot S_T, where S >= 0
// Returns undiscoutned cash flow; needs to be discounted by pricer
struct Payoff {
    virtual double operator()(double S) const = 0;
    virtual ~Payoff() = default;
};

// Payoff of European call option
// CallPayoff(double strike): requires `strike` >= 0 (asserted)
// operator()(double S): returns max(S-strike, 0)
class CallPayoff final: public Payoff {
    public:
        explicit CallPayoff(double strike);
        double operator()(double S) const override;
    private:
        double K_;
};

// Payoff of European put option
// PutPayoff(double strike): requires `strike` >= 0 (asserted)
// operator()(double S): returns max(strike-S, 0)
class PutPayoff final: public Payoff {
    public:
        explicit PutPayoff(double strike);
        double operator()(double S) const override;
    private:
        double K_;
};

}   // namespace quant::pricing
