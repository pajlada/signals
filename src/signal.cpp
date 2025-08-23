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

TEST(Signal, DisconnectAll)
{
    Signal<int> signal;

    int a = 0;
    auto cb = [&](int i) { a += i; };

    EXPECT_EQ(a, 0);

    signal.invoke(1);

    EXPECT_EQ(a, 0);

    auto connA = signal.connect(cb);
    auto connB = signal.connect(cb);

    signal.invoke(1);
    EXPECT_EQ(a, 2);

    Connection connACopy = connA;

    signal.invoke(1);
    EXPECT_EQ(a, 4);

    EXPECT_EQ(connA.getSubscriberRefCount().connected, true);
    EXPECT_EQ(connA.getSubscriberRefCount().count, 2);
    signal.disconnectAll();

    signal.invoke(1);
    EXPECT_EQ(a, 4);
    EXPECT_EQ(connA.getSubscriberRefCount().connected, false);
    EXPECT_EQ(connA.getSubscriberRefCount().count, 0);
}
