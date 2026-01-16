#include <pajlada/signals/signal.hpp>

#include <gtest/gtest.h>

#include <string>

using namespace pajlada::Signals;

TEST(BoltSignal, InvokeOwned)
{
    BoltSignal<std::string> signal;

    bool called = false;
    auto consumerOwned = [&](std::string s) {
        EXPECT_EQ(s, "Yes, this is a really long long string!");
        called = true;
    };
    auto consumerConstRef = [&](const std::string &s) {
        EXPECT_EQ(s, "Yes, this is a really long long string!");
        called = true;
    };

    signal.connect(consumerOwned);
    signal.connect(consumerConstRef);
    signal.connect(consumerOwned);
    signal.connect(consumerConstRef);
    signal.invoke("Yes, this is a really long long string!");
    EXPECT_TRUE(called);
    called = false;

    signal.connect(consumerOwned);
    signal.connect(consumerConstRef);
    signal.connect(consumerOwned);
    signal.connect(consumerConstRef);
    std::string owned = "Yes, this is a really long long string!";
    signal.invoke(owned);
    EXPECT_TRUE(called);
}
