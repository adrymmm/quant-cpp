#include "quant/book/order_book.hpp"

namespace quant::book {

[[nodiscard]] std::optional<Price> OrderBook::best_bid() const {
    if (bids_.empty()) return std::nullopt;
    return bids_.begin()->first;
}

[[nodiscard]] std::optional<Price> OrderBook::best_ask() const {
    if (asks_.empty()) return std::nullopt;
    return asks_.begin()->first;
}

template <typename BookSide>
std::vector<Trade> OrderBook::match_against(BookSide& opposite, OrderId id, Side side, Quantity& remainder, std::optional<Price> limit ) {
    std::vector<Trade> trades;
    while (remainder > 0) {
        if (opposite.empty()) return trades;
        
        auto it = opposite.begin();
        // unpack it->first, it-> second
        auto& [price, level] = *it;

        // market (no limit), crosses, doesn't cross
        bool crosses = !limit || (side == Side::Buy && price <= *limit) || (side == Side::Sell && price >= *limit);

        if (!crosses) return trades;
        
        auto& resting =  level.front();
        const Quantity fill = std::min(remainder, resting.quantity);

        Trade trade { .aggressor_id = id, .resting_id = resting.id,
            .price = resting.price, .quantity = fill, .aggressor_side = side
        };
        trades.push_back(trade);
        remainder -= fill;
        resting.quantity -= fill;
        
        // pop from list and erase from idx if resting was drained
        if (resting.quantity == 0) {
            index_.erase(resting.id);
            // pop from Levl
            level.pop_front();
            // erase whole price level if empty
            if (level.empty()) opposite.erase(it);
        }
    }
    return trades;
}


[[nodiscard]] std::vector<Trade> OrderBook::add_limit(const Order& order) {
}

} //namespace quant::book 

