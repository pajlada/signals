#include <gtest/gtest.h>

#include <pajlada/signals/signal.hpp>

using namespace pajlada::Signals;

TEST(SelfDisconnectingSignal, MultipleConnects)
{
    NoArgSelfDisconnectingSignal signal;

    int a = 0;
    int b = 0;

    signal.connect([&] {
        a++;
        return a == 1;
    });

    signal.connect([&] {
        b++;
        return b == 2;
    });

    signal.invoke();

    EXPECT_EQ(a, 1);
    EXPECT_EQ(b, 1);

    signal.invoke();

    EXPECT_EQ(a, 1);
    EXPECT_EQ(b, 2);

    signal.invoke();

    EXPECT_EQ(a, 1);
    EXPECT_EQ(b, 2);
}
