#include "stream.hpp"
#include "id_pool.hpp"
#include "quant/book/order_book.hpp"
#include <random>
#include <stdexcept>
#include <cmath>

namespace {
    enum class Kind { Limit, Cancel, Market};
} // namespace

namespace quant::bench {

std::vector<Message> generate_stream(const StreamParams& params) {
    if (params.limit_pct < 0)
        throw std::invalid_argument("limit_pct must be >= 0");
    if (params.cancel_pct < 0)
        throw std::invalid_argument("cancel_pct must be >= 0");
    if (params.market_pct < 0)
        throw std::invalid_argument("market_pct must be >= 0");
    if (params.limit_pct + params.cancel_pct + params.market_pct != 100)
        throw std::invalid_argument("limit_pct + cancel_pct + market_pct must equal 100");
    if (!std::isfinite(params.mean_geom_offset) || params.mean_geom_offset <= 0)
        throw std::invalid_argument("mean_geom_offset must be finite and > 0");
    if (params.max_qty < 1)
        throw std::invalid_argument("max_qty must be >= 1");

    std::mt19937_64 rng(params.seed);
    std::vector<Message> out;
    out.reserve(params.count);
    quant::book::OrderBook book;
    quant::bench::IdPool pool;

    std::discrete_distribution<> dis({static_cast<double>(params.limit_pct), static_cast<double>(params.cancel_pct), static_cast<double>(params.market_pct)});
    for (std::size_t i = 0; i < params.count; i++) {
        constexpr std::array<Kind, 3> kind_lookup{Kind::Limit, Kind::Cancel, Kind::Market};
        auto r = dis(rng);

        // Index into kind array with discrete prob variable based on pcts
        const Kind kind = kind_lookup[r];
    }
    return out;

}

} // namespace quant::bench
