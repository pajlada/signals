#include <pajlada/signals.hpp>

#include <gtest/gtest.h>

#include <vector>

using namespace pajlada::Signals;

TEST(SignalHolder, A)
{
    Signal<int> incrementSignal;
    int a = 0;
    auto IncrementA = [&a](int incrementBy) {
        a += incrementBy;  //
    };
    EXPECT_EQ(a, 0);

    {
        SignalHolder holder;

        holder.managedConnect(incrementSignal, IncrementA);

        incrementSignal.invoke(1);
        EXPECT_EQ(a, 1);

        holder.managedConnect(incrementSignal, IncrementA);

        incrementSignal.invoke(1);
        EXPECT_EQ(a, 3);
    }

    incrementSignal.invoke(1);
    EXPECT_EQ(a, 3);
}

TEST(SignalHolder, Clear)
{
    Signal<int> incrementSignal;
    int a = 0;
    auto IncrementA = [&a](int incrementBy) {
        a += incrementBy;  //
    };
    EXPECT_EQ(a, 0);

    {
        SignalHolder holder;

        holder.managedConnect(incrementSignal, IncrementA);

        incrementSignal.invoke(1);
        EXPECT_EQ(a, 1);

        holder.managedConnect(incrementSignal, IncrementA);

        incrementSignal.invoke(1);
        EXPECT_EQ(a, 3);

        holder.clear();

        incrementSignal.invoke(1);
        EXPECT_EQ(a, 3);

        holder.managedConnect(incrementSignal, IncrementA);

        incrementSignal.invoke(1);
        EXPECT_EQ(a, 4);
    }

    incrementSignal.invoke(1);
    EXPECT_EQ(a, 4);
}
