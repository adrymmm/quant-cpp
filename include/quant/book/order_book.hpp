#pragma once
#include <cstdint>
#include <functional>
#include <list>
#include <map>
#include <optional>
#include <unordered_map>
#include <vector>

namespace quant::book {
using Price    = std::int64_t;   // ticks (cents)
using Quantity = std::int64_t;
using OrderId  = std::uint64_t;

enum class Side : std::uint8_t { Buy, Sell };

struct Order {
    Price price;
    Quantity quantity;
    OrderId id;
    Side side;

    bool operator==(const Order&) const = default;
};

static_assert(sizeof(Order) == 32);

struct Trade {
    OrderId aggressor_id;
    OrderId resting_id;
    Price price;
    Quantity quantity;
    Side aggressor_side;

    bool operator==(const Trade&) const = default;
};


class OrderBook {
    public:
        // highest resting buy price; std::nullopt if no bids
        [[nodiscard]] std::optional<Price> best_bid() const;

        // highest resting sell price; std::nullopt if no asks
        [[nodiscard]] std::optional<Price> best_ask() const;

        // matching at resting prices against opposite side; remainder rests, fills in order
        // throw std::invalid_argument if quantity <= 0 or id already in book
        [[nodiscard]] std::vector<Trade> add_limit(const Order& order);

        // matching at resting prices against opposite side; remainder dropped, fills in order
        // throws std::invalid_argument if quantity <= 0
        [[nodiscard]] std::vector<Trade> add_market(OrderId id, Side side, Quantity quantity);

        // remove resting order; return false if id is not in book
        bool cancel(OrderId id);

    private:
        using Level = std::list<Order>;
        struct OrderLocation{  Price price; Level::iterator it; Side side; };
        template <typename BookSide>
        // remainder passed by value to be modified in-place
        std::vector<Trade> match_against(BookSide& opposite, OrderId id, Side side, Quantity& remainder, std::optional<Price> limit );
        std::map<Price, Level, std::greater<Price>> bids_;
        std::map<Price, Level> asks_;
        std::unordered_map<OrderId, OrderLocation> index_;
};
} // namespace quant::book
