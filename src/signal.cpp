#include "pajlada/signals/connection.hpp"
#include <pajlada/signals/signal.hpp>

#include <gtest/gtest.h>

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
