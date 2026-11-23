#pragma once
#include "quant/book/order_book.hpp"
#include <vector>
#include <unordered_map>
#include <cstddef>
#include <optional>
#include <random>

namespace quant::bench {

using OrderId = quant::book::OrderId;

class IdPool {
public:
    // adds orderid to ids_ vector; returns false if id present
    bool add(OrderId id) {
        [[maybe_unused]] auto [it, inserted] = idx_of_.try_emplace(id, ids_.size());
        if (!inserted) return false;
        ids_.push_back(id);
        return true;
    }

    // removes orderid from ids_ vector; returns false if id not present
    bool remove(OrderId id) {
        auto it = idx_of_.find(id);
        if (it == idx_of_.end()) return false;

        auto i = it->second;
        auto last = ids_.back();

        // overwrite old id with last element and pop last 
        ids_[i] = last;
        ids_.pop_back();

        // replace moved member's idx with removed member's and erase old key
        idx_of_.at(last) = i;
        idx_of_.erase(id);
        return true;
    }
    // pick a random order id by drawing a random idx; returns nullopt if pool is empty 
    [[nodiscard]] std::optional<OrderId> pick(std::mt19937_64& rng) const {
        if (ids_.empty()) return std::nullopt;

        std::uniform_int_distribution<std::size_t> dis(0, ids_.size() - 1);
        return ids_[dis(rng)];
    }

    [[nodiscard]] std::size_t size() const {
        return ids_.size();
    }

    [[nodiscard]] bool empty() const {
        return ids_.empty();
    }

private:
    // order ids
    std::vector<OrderId> ids_;
    // indices of order ids
    std::unordered_map<OrderId, std::size_t> idx_of_;
};
} // namespace quant::bench
