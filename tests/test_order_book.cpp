#include "quant/book/order_book.hpp"
#include <gtest/gtest.h>
#include <optional>

namespace qb = quant::book;

TEST(BookTest, EmptyBookHasNoBestPrices) {
    qb::OrderBook book;
    EXPECT_EQ(book.best_ask(), std::nullopt);
    EXPECT_EQ(book.best_bid(), std::nullopt);
}

TEST(BookTest, RestingBidSetsBestBid) {
    qb::OrderBook book;
    const qb::Order buy_order{.price = 100, .quantity = 20, .id=1, .side=qb::Side::Buy};
    std::vector<qb::Trade> trades = book.add_limit(buy_order);
    EXPECT_TRUE(trades.empty());
    EXPECT_EQ(book.best_bid(), 100);
    EXPECT_EQ(book.best_ask(), std::nullopt);
}

TEST(BookTest, CrossingLimitTradesAtRestingPrice) {
    qb::OrderBook book;
    const qb::Order buy_order{.price = 100, .quantity = 20, .id=1, .side=qb::Side::Buy};
    const qb::Order sell_order{.price = 95, .quantity = 20, .id=2, .side=qb::Side::Sell};
    std::vector<qb::Trade> trades1 = book.add_limit(sell_order);
    std::vector<qb::Trade> trades2 = book.add_limit(buy_order);

    ASSERT_EQ(trades2.size(), 1u);
    EXPECT_EQ(trades2[0], (qb::Trade{ .aggressor_id = 1, .resting_id = 2,
                                      .price = 95,       .quantity = 20,
                                      .aggressor_side = qb::Side::Buy,}));
    EXPECT_TRUE(trades1.empty());
    EXPECT_EQ(book.best_ask(), std::nullopt);
    EXPECT_EQ(book.best_bid(), std::nullopt);
}

TEST(BookTest, MultiLevelSweep) {
    qb::OrderBook book;
    const qb::Order buy_order{.price = 100, .quantity = 40, .id=1, .side=qb::Side::Buy};
    const qb::Order sell_order1{.price = 95, .quantity = 30, .id=2, .side=qb::Side::Sell};
    const qb::Order sell_order2{.price = 94, .quantity = 20, .id=3, .side=qb::Side::Sell};

    std::vector<qb::Trade> trades1 = book.add_limit(sell_order1);
    std::vector<qb::Trade> trades2 = book.add_limit(sell_order2);

    std::vector<qb::Trade> trades3 = book.add_limit(buy_order);

    ASSERT_EQ(trades3.size(), 2u);

    // First trade should be 94 
    EXPECT_EQ(trades3[0], (qb::Trade{ .aggressor_id = 1, .resting_id = 3,
                                      .price = 94,       .quantity = 20,
                                      .aggressor_side = qb::Side::Buy,}));

    EXPECT_EQ(trades3[1], (qb::Trade{ .aggressor_id = 1, .resting_id = 2,
                                      .price = 95,       .quantity = 20,
                                      .aggressor_side = qb::Side::Buy,}));

    EXPECT_TRUE(trades1.empty());
    EXPECT_TRUE(trades2.empty());

    EXPECT_EQ(book.best_ask(), 95);
    EXPECT_EQ(book.best_bid(), std::nullopt);
}
