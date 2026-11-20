#include "quant/book/order_book.hpp"
#include <stdexcept>
#include <algorithm>

namespace quant::book {

std::optional<Price> OrderBook::best_bid() const {
    if (bids_.empty()) return std::nullopt;
    return bids_.begin()->first;
}

std::optional<Price> OrderBook::best_ask() const {
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

template <typename BookSide>
void OrderBook::rest(BookSide& own, const Order& order, Quantity remainder) {
    if (remainder <= 0) return;

    auto& level = own[order.price]; //get price level or create new one
    Order resting = order;
    resting.quantity = remainder;
    // appending to end of price level list and get iterator for idx
    auto it = level.insert(level.end(), resting);
    index_.emplace(order.id, OrderLocation{order.price, it, order.side});
}

std::vector<Trade> OrderBook::add_limit(const Order& order) {
    if (order.quantity <= 0) throw std::invalid_argument("quantity must be positive");
    if (index_.contains(order.id)) throw std::invalid_argument("duplicate order id");
    std::vector<Trade> trades;
    Quantity remainder = order.quantity;
    if (order.side == Side::Buy) {
        trades = match_against(asks_, order.id, order.side, remainder, order.price);
        rest(bids_, order, remainder);
    } 
    else {
        trades = match_against(bids_, order.id, order.side, remainder, order.price);
        rest(asks_, order, remainder);
    }
    return trades;
}
template <typename BookSide> void OrderBook::erase_order(BookSide& side, const OrderLocation& loc) {
    auto level_it = side.find(loc.price);
    auto& level = level_it->second;
    //erase order from level
    level.erase(loc.it);
    // if price level empty remove from map
    if (level.empty()) {
        side.erase(level_it);
    }
}

bool OrderBook::cancel(OrderId id) {
    auto it = index_.find(id);
    // if id not found .find returns .end()
    if (it == index_.end()) return false;
    const auto& loc = it->second;

    if (loc.side == Side::Buy) {
        erase_order(bids_, loc);
    }
    else {
        erase_order(asks_, loc);
    }
    index_.erase(id);
    return true;
}

} //namespace quant::book 

