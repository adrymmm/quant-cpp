#pragma once
#include <cstdint>

namespace quant::book {
using Price    = std::int64_t;   // ticks (cents)
using Quantity = std::int64_t;
using OrderId  = std::uint64_t;

enum class Side : std::uint8_t { Buy, Sell };

struct Order {
    Side side;
    Price price;
    Quantity quantity;
    OrderId id;
};

} // namespace quant::book 
