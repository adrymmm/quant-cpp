# quant-cpp

Option pricing and a limit order book in C++20.

## Contents

- **Payoffs**: European call and put behind a common `Payoff` interface.
- **Black–Scholes**: closed-form European call and put.
- **Monte Carlo**: European pricer for any `Payoff`, returning the price and its standard error.
- **Order book**: price-time priority limit order book with limit orders, market
  orders (IOC: any unfilled remainder is dropped) and cancel by ID.

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

Order book tests cover time priority within a level, multi-level sweeps, partial
fills, cancels, market orders and invalid input. All tests also run under ASan
and UBSan (`-DSANITIZER=address`).
