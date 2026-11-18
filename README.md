# quant-cpp

Option pricing in C++20: Black–Scholes and Monte Carlo.

## Contents

- **Payoffs**: European call and put behind a common `Payoff` interface.
- **Black–Scholes**: closed-form European call and put.
- **Monte Carlo**: European pricer for any `Payoff`, returning the price and its standard error.

## Build and test

```
cmake -S . -B build
cmake --build build
ctest --test-dir build
```

## Validation

Black–Scholes prices are checked against Hull's worked examples and put–call
parity. Monte Carlo call and put prices are checked against the closed form to
within three standard errors (100,000 paths, fixed seed).
