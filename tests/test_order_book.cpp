#include "quant/book/order_book.hpp"
#include <gtest/gtest.h>
#include <optional>
#include <stdexcept>

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

TEST(BookTest, PartialFillRestAtFront) {
    qb::OrderBook book;
    const qb::Order buy_order{.price = 100, .quantity = 4, .id=1, .side=qb::Side::Buy};
    const qb::Order sell_order1{.price = 100, .quantity = 10, .id=2, .side=qb::Side::Sell};
    const qb::Order sell_order2{.price = 100, .quantity = 5, .id=3, .side=qb::Side::Sell};

    const auto trades1 = book.add_limit(sell_order1);
    const auto trades2 = book.add_limit(sell_order2);
    const auto trades3 = book.add_limit(buy_order);
    
    ASSERT_EQ(trades3.size(), 1u);
    EXPECT_TRUE(trades1.empty());
    EXPECT_TRUE(trades2.empty());

    EXPECT_EQ(trades3[0], (qb::Trade{ .aggressor_id = 1, .resting_id = 2,
                                      .price = 100,       .quantity = 4,
                                      .aggressor_side = qb::Side::Buy,}));
    
    const qb::Order buy_order1{.price = 100, .quantity = 8, .id=4, .side=qb::Side::Buy};
    std::vector<qb::Trade> trades4 = book.add_limit(buy_order1);

    // we should hit first sell up to 6 and second sell up to 2
    ASSERT_EQ(trades4.size(), 2u);

    EXPECT_EQ(trades4[0], (qb::Trade{ .aggressor_id = 4, .resting_id = 2,
                                      .price = 100,       .quantity = 6,
                                      .aggressor_side = qb::Side::Buy,}));

    EXPECT_EQ(trades4[1], (qb::Trade{ .aggressor_id = 4, .resting_id = 3,
                                      .price = 100,       .quantity = 2,
                                      .aggressor_side = qb::Side::Buy,}));
}

TEST(BookTest, InvalidLimitThrows) {
    qb::OrderBook book;
    const qb::Order zero{.price = 100, .quantity = 0, .id=1, .side=qb::Side::Buy};
    const qb::Order neg{.price = 100, .quantity = -2, .id=2, .side=qb::Side::Buy};
    const qb::Order valid{.price = 100, .quantity = 10, .id=3, .side=qb::Side::Buy};
    const qb::Order dup_id{.price = 100, .quantity = 20, .id=3, .side=qb::Side::Sell};

    EXPECT_THROW((void)book.add_limit(zero), std::invalid_argument);
    EXPECT_THROW((void)book.add_limit(neg), std::invalid_argument);

    EXPECT_FALSE(book.best_bid());
    
    const auto valid_trade = book.add_limit(valid);
    EXPECT_TRUE(valid_trade.empty());
    EXPECT_THROW((void)book.add_limit(dup_id), std::invalid_argument);
    const qb::Order probe{.price = 100, .quantity = 20, .id=4, .side=qb::Side::Sell};
    const auto probe_trade = book.add_limit(probe);
    ASSERT_EQ(probe_trade.size(), 1u);

    EXPECT_EQ(probe_trade[0], (qb::Trade{ .aggressor_id = 4, .resting_id = 3,
                                      .price = 100,       .quantity = 10,
                                      .aggressor_side = qb::Side::Sell,}));
}
