#pragma once
#include <vector>
#include <cstdint>
#include <variant>
#include <cstddef>
#include "quant/book/order_book.hpp"
namespace quant::bench {

namespace qb = quant::book;

struct Limit {
    qb::Side side;
    qb::OrderId id;
    qb::Price price;
    qb::Quantity quantity;
};

struct Market {
    qb::Side side;
    qb::Quantity quantity;
};

struct Cancel {
    qb::OrderId id;
};

using Message = std::variant<Limit, Market, Cancel>;

struct StreamParams {
    std::size_t count = 1'000'000;
    int limit_pct = 50;
    int cancel_pct = 45;
    int market_pct = 5;
    double mean_geom_offset = 2.00;
    qb::Price fixed_mid = 10'000;
    qb::Quantity max_qty = 100;
    std::uint64_t seed = 42;
};

// Returns count messages for replay into a fresh book.
// Same params (incl. seed) -> same stream.
// Mix ~ limit/cancel/market pcts.
// Cancels hit live ids; none live -> limit instead.
// Throws std::invalid_argument on bad params.
std::vector<Message> generate_stream(const StreamParams& params);

} // namespace quant::bench
