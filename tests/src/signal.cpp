#include "pajlada/signals/connection.hpp"
#include <pajlada/signals/signal.hpp>

#include <gtest/gtest.h>

#include <string>

using namespace pajlada::Signals;

TEST(Signal, SingleConnect)
{
    Signal<int> incrementSignal;

    int a = 0;

    auto IncrementA = [&a](int incrementBy) {
        a += incrementBy;  //
    };

    EXPECT_EQ(a, 0);

    incrementSignal.invoke(1);

    EXPECT_EQ(a, 0);

    auto connA = incrementSignal.connect(IncrementA);

    incrementSignal.invoke(1);

    EXPECT_EQ(a, 1);

    incrementSignal.invoke(2);

    EXPECT_EQ(a, 3);
}

TEST(Signal, MultipleConnect)
{
    Signal<int> incrementSignal;

    int a = 0;

    auto IncrementA = [&a](int incrementBy) {
        a += incrementBy;  //
    };

    EXPECT_EQ(a, 0);

    incrementSignal.invoke(1);

    EXPECT_EQ(a, 0);

    auto connA = incrementSignal.connect(IncrementA);
    auto connB = incrementSignal.connect(IncrementA);

    incrementSignal.invoke(1);

    EXPECT_EQ(a, 2);

    incrementSignal.invoke(2);

    EXPECT_EQ(a, 6);
}

TEST(Signal, InvokeOwned)
{
    Signal<std::string> signal;

    bool called = false;
    auto consumerOwned = [&](std::string s) {
        EXPECT_EQ(s, "Yes, this is a really long long string!");
        called = true;
    };
    auto consumerConstRef = [&](const std::string &s) {
        EXPECT_EQ(s, "Yes, this is a really long long string!");
        called = true;
    };
    auto connA = signal.connect(consumerOwned);
    auto connB = signal.connect(consumerConstRef);
    auto connC = signal.connect(consumerOwned);
    auto connD = signal.connect(consumerConstRef);

    signal.invoke("Yes, this is a really long long string!");
    EXPECT_TRUE(called);
    called = false;

    std::string owned = "Yes, this is a really long long string!";
    signal.invoke(owned);
    EXPECT_TRUE(called);
}
