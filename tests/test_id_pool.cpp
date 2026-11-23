#include "id_pool.hpp"
#include <gtest/gtest.h>
#include <random>
#include <set>

using IdPool = quant::bench::IdPool;
using OrderId = quant::bench::OrderId;

TEST(IdPool, AddExisting) {
    IdPool ip;
    EXPECT_TRUE(ip.add(1));
    EXPECT_TRUE(ip.add(2));
    EXPECT_FALSE(ip.add(1));
    EXPECT_EQ(ip.size(), 2u);
}

TEST(IdPool, RemoveNonExisting) {
    IdPool ip;
    EXPECT_TRUE(ip.add(1));
    EXPECT_TRUE(ip.add(2));
    EXPECT_TRUE(ip.remove(2));
    EXPECT_EQ(ip.size(), 1u);

    EXPECT_FALSE(ip.remove(3));
    EXPECT_EQ(ip.size(), 1u);
}

TEST(IdPool, EmptyPool) {
    IdPool ip;
    EXPECT_TRUE(ip.add(1));
    EXPECT_TRUE(ip.add(2));
    EXPECT_EQ(ip.size(), 2u);

    EXPECT_TRUE(ip.remove(1));
    EXPECT_TRUE(ip.remove(2));
    EXPECT_TRUE(ip.empty());
}

TEST(IdPool, RemoveFromMiddle) {
    IdPool ip;
    EXPECT_TRUE(ip.add(1));
    EXPECT_TRUE(ip.add(2));
    EXPECT_TRUE(ip.add(3));

    EXPECT_TRUE(ip.remove(1));
    EXPECT_TRUE(ip.remove(3));
    EXPECT_FALSE(ip.remove(1));

    EXPECT_EQ(ip.size(), 1u);
}

TEST(IdPool, RemoveLastThenReAdd) {
    IdPool ip;
    EXPECT_TRUE(ip.add(1));
    EXPECT_TRUE(ip.add(2));

    EXPECT_TRUE(ip.remove(2));
    EXPECT_TRUE(ip.add(2));

    EXPECT_EQ(ip.size(), 2u);
}

TEST(IdPool, PickWithEmptyPool) {
    std::mt19937_64 rng{42};
    IdPool ip;

    EXPECT_EQ(ip.pick(rng), std::nullopt);
    EXPECT_FALSE(ip.pick(rng).has_value());
}

TEST(IdPool, AlwaysLive) {
    std::mt19937_64 rng{42};
    IdPool ip;
    EXPECT_TRUE(ip.add(1));
    EXPECT_TRUE(ip.add(2));
    EXPECT_TRUE(ip.add(3));
    EXPECT_TRUE(ip.add(4));
    EXPECT_TRUE(ip.add(5));

    EXPECT_TRUE(ip.remove(2));
    EXPECT_TRUE(ip.remove(4));
    const std::set<OrderId> live{1, 3, 5};

    // id from pick must never be 2 or 4
    for (int i = 0; i < 100; i++) {
        auto id = ip.pick(rng);
        ASSERT_TRUE(id.has_value());
        EXPECT_TRUE(live.contains(*id)) << "picked " << *id;
    }
}

TEST(IdPool, ReachesEverything) {
    std::mt19937_64 rng{42};
    IdPool ip;
    EXPECT_TRUE(ip.add(1));
    EXPECT_TRUE(ip.add(2));
    EXPECT_TRUE(ip.add(3));
    EXPECT_TRUE(ip.add(4));

    std::set<OrderId> ids;
    for (int i = 0; i < 1000; i++) {
        auto id = ip.pick(rng);
        ASSERT_TRUE(id.has_value());
        ids.insert(*id);
    }
    EXPECT_EQ(ids.size(), 4u);
    EXPECT_EQ(ids, (std::set<OrderId>{1, 2, 3, 4}));
}

TEST(IdPool, PickDoesntChangePool) {
    std::mt19937_64 rng{42};
    IdPool ip;
    EXPECT_TRUE(ip.add(1));
    EXPECT_TRUE(ip.add(2));
    EXPECT_TRUE(ip.add(3));

    EXPECT_EQ(ip.size(), 3u);
    [[maybe_unused]] auto id = ip.pick(rng);
    EXPECT_EQ(ip.size(), 3u);

}
