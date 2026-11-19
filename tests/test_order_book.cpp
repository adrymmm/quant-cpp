#include "quant/book/order_book.hpp"
#include <gtest/gtest.h>
#include <optional>

namespace qb = quant::book;

TEST(BookTest, EmptyBookHasNoBestPrices) {
    qb::OrderBook book;
    EXPECT_EQ(book.best_ask(), std::nullopt);
    EXPECT_EQ(book.best_bid(), std::nullopt);
}

