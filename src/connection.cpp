#include <pajlada/signals/signal.hpp>

#include <gtest/gtest.h>

using namespace pajlada::Signals;

TEST(Connection, IsConnected)
{
    Signal<int> incrementSignal;
    int a = 0;
    auto IncrementA = [&a](int incrementBy) {
        a += incrementBy;  //
    };
    EXPECT_EQ(a, 0);

    auto conn = incrementSignal.connect(IncrementA);

    incrementSignal.invoke(1);

    EXPECT_EQ(a, 1);

    EXPECT_TRUE(conn.isConnected());
    conn.disconnect();
    EXPECT_FALSE(conn.isConnected());
    conn.disconnect();
    EXPECT_FALSE(conn.isConnected());

    incrementSignal.invoke(1);

    EXPECT_EQ(a, 1);
}

TEST(Connection, Blocking)
{
    Signal<int> incrementSignal;
    int a = 0;
    auto IncrementA = [&a](int incrementBy) {
        a += incrementBy;  //
    };
    EXPECT_EQ(a, 0);

    auto conn = incrementSignal.connect(IncrementA);

    incrementSignal.invoke(1);

    EXPECT_EQ(a, 1);

    EXPECT_FALSE(conn.isBlocked());
    EXPECT_TRUE(conn.block());
    EXPECT_TRUE(conn.isBlocked());
    EXPECT_FALSE(conn.block());
    EXPECT_TRUE(conn.isBlocked());

    incrementSignal.invoke(1);

    EXPECT_EQ(a, 1);

    EXPECT_TRUE(conn.unblock());
    EXPECT_FALSE(conn.isBlocked());
    EXPECT_FALSE(conn.unblock());
    EXPECT_FALSE(conn.isBlocked());

    incrementSignal.invoke(2);

    EXPECT_EQ(a, 3);
}

TEST(Connection, BlockDisconnected)
{
    Signal<int> incrementSignal;
    int a = 0;
    auto IncrementA = [&a](int incrementBy) {
        a += incrementBy;  //
    };
    EXPECT_EQ(a, 0);

    auto conn = incrementSignal.connect(IncrementA);

    incrementSignal.invoke(1);

    EXPECT_EQ(a, 1);

    EXPECT_TRUE(conn.isConnected());
    conn.disconnect();
    EXPECT_FALSE(conn.isConnected());

    incrementSignal.invoke(1);

    EXPECT_EQ(a, 1);

    EXPECT_FALSE(conn.isBlocked());
    EXPECT_FALSE(conn.block());
    EXPECT_FALSE(conn.isBlocked());
    EXPECT_FALSE(conn.unblock());
    EXPECT_FALSE(conn.isBlocked());
}

TEST(Connection, Copying)
{
    Signal<int> incrementSignal;
    int a = 0;
    auto IncrementA = [&a](int incrementBy) {
        a += incrementBy;  //
    };
    EXPECT_EQ(a, 0);

    auto conn = incrementSignal.connect(IncrementA);

    EXPECT_TRUE(conn.getSubscriberRefCount().connected);
    EXPECT_EQ(conn.getSubscriberRefCount().count, 1);

    incrementSignal.invoke(1);
    EXPECT_EQ(a, 1);

    // Test copying of connection
    auto connCopy = conn;
    EXPECT_TRUE(conn.getSubscriberRefCount().connected);
    EXPECT_EQ(conn.getSubscriberRefCount().count, 2);

    incrementSignal.invoke(1);
    EXPECT_EQ(a, 2);

    EXPECT_TRUE(connCopy.disconnect());

    incrementSignal.invoke(1);
    EXPECT_EQ(a, 3);

    EXPECT_TRUE(conn.getSubscriberRefCount().connected);
    EXPECT_EQ(conn.getSubscriberRefCount().count, 1);

    EXPECT_TRUE(conn.disconnect());

    EXPECT_FALSE(conn.getSubscriberRefCount().connected);

    incrementSignal.invoke(1);
    EXPECT_EQ(a, 3);
}

TEST(Connection, AssignmentOperatorCopySelf)
{
    Signal<int> incrementSignal;
    int a = 0;
    auto IncrementA = [&a](int incrementBy) {
        a += incrementBy;  //
    };
    EXPECT_EQ(a, 0);

    auto conn = incrementSignal.connect(IncrementA);

    EXPECT_TRUE(conn.getSubscriberRefCount().connected);
    EXPECT_EQ(conn.getSubscriberRefCount().count, 1);

    incrementSignal.invoke(1);
    EXPECT_EQ(a, 1);

    // Test copying of connection
    auto connCopy = conn;
    EXPECT_TRUE(conn.getSubscriberRefCount().connected);
    EXPECT_EQ(conn.getSubscriberRefCount().count, 2);

    incrementSignal.invoke(1);
    EXPECT_EQ(a, 2);

    connCopy = connCopy;

    EXPECT_TRUE(conn.getSubscriberRefCount().connected);
    EXPECT_EQ(conn.getSubscriberRefCount().count, 2);

    incrementSignal.invoke(1);
    EXPECT_EQ(a, 3);

    EXPECT_TRUE(connCopy.disconnect());

    incrementSignal.invoke(1);
    EXPECT_EQ(a, 4);

    EXPECT_TRUE(conn.getSubscriberRefCount().connected);
    EXPECT_EQ(conn.getSubscriberRefCount().count, 1);

    EXPECT_TRUE(conn.disconnect());

    EXPECT_FALSE(conn.getSubscriberRefCount().connected);

    incrementSignal.invoke(1);
    EXPECT_EQ(a, 4);
}

TEST(Connection, AssignmentOperatorMove)
{
    Signal<int> incrementSignal;
    int a = 0;
    auto IncrementA = [&a](int incrementBy) {
        a += incrementBy;  //
    };
    EXPECT_EQ(a, 0);

    auto conn = incrementSignal.connect(IncrementA);

    EXPECT_TRUE(conn.getSubscriberRefCount().connected);
    EXPECT_EQ(conn.getSubscriberRefCount().count, 1);

    incrementSignal.invoke(1);
    EXPECT_EQ(a, 1);

    // Test copying of connection
    auto yoinkedConn = std::move(conn);
    EXPECT_FALSE(conn.getSubscriberRefCount().connected);
    EXPECT_EQ(conn.getSubscriberRefCount().count, 0);
    EXPECT_TRUE(yoinkedConn.getSubscriberRefCount().connected);
    EXPECT_EQ(yoinkedConn.getSubscriberRefCount().count, 1);

    incrementSignal.invoke(1);
    EXPECT_EQ(a, 2);

    EXPECT_TRUE(yoinkedConn.disconnect());

    incrementSignal.invoke(1);
    EXPECT_EQ(a, 2);
}

TEST(Connection, AssignmentOperatorMoveSelf)
{
    Signal<int> incrementSignal;
    int a = 0;
    auto IncrementA = [&a](int incrementBy) {
        a += incrementBy;  //
    };
    EXPECT_EQ(a, 0);

    auto conn = incrementSignal.connect(IncrementA);

    EXPECT_TRUE(conn.getSubscriberRefCount().connected);
    EXPECT_EQ(conn.getSubscriberRefCount().count, 1);

    incrementSignal.invoke(1);
    EXPECT_EQ(a, 1);

    // Test copying of connection
    auto yoinkedConn = std::move(conn);
    EXPECT_FALSE(conn.getSubscriberRefCount().connected);
    EXPECT_EQ(conn.getSubscriberRefCount().count, 0);
    EXPECT_TRUE(yoinkedConn.getSubscriberRefCount().connected);
    EXPECT_EQ(yoinkedConn.getSubscriberRefCount().count, 1);

    incrementSignal.invoke(1);
    EXPECT_EQ(a, 2);

    yoinkedConn = std::move(yoinkedConn);

    EXPECT_TRUE(yoinkedConn.getSubscriberRefCount().connected);
    EXPECT_EQ(yoinkedConn.getSubscriberRefCount().count, 1);

    incrementSignal.invoke(1);
    EXPECT_EQ(a, 3);

    EXPECT_TRUE(yoinkedConn.disconnect());

    incrementSignal.invoke(1);
    EXPECT_EQ(a, 3);
}
