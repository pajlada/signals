#include "pajlada/signals/connection.hpp"

#include <pajlada/signals/signal.hpp>

#include <gtest/gtest.h>

using namespace pajlada::Signals;

TEST(Connection, IsConnected)
{
    Connection conn;

    int a = 0;
    {
        Signal<int> incrementSignal;
        auto IncrementA = [&a](int incrementBy) {
            a += incrementBy;  //
        };
        EXPECT_EQ(a, 0);

        conn = incrementSignal.connect(IncrementA);

        incrementSignal.invoke(1);

        EXPECT_EQ(a, 1);

        EXPECT_TRUE(conn.isConnected());
        conn.disconnect();
        EXPECT_FALSE(conn.isConnected());
        conn.disconnect();
        EXPECT_FALSE(conn.isConnected());

        incrementSignal.invoke(1);
    }
    EXPECT_FALSE(conn.isConnected());
    EXPECT_EQ(conn.getSubscriberRefCount().connected, false);
    EXPECT_EQ(conn.getSubscriberRefCount().count, 0);

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

TEST(Connection, AssignmentOperatorCopyOther)
{
    Signal<int> incrementSignal;
    int testValue = 0;
    auto IncrementA = [&testValue](int incrementBy) {
        testValue += incrementBy;  //
    };
    EXPECT_EQ(testValue, 0);

    auto conn = incrementSignal.connect(IncrementA);

    EXPECT_TRUE(conn.getSubscriberRefCount().connected);
    EXPECT_EQ(conn.getSubscriberRefCount().count, 1);

    incrementSignal.invoke(1);
    EXPECT_EQ(testValue, 1);

    // Test copying of connection
    Connection otherConn;
    EXPECT_FALSE(otherConn.getSubscriberRefCount().connected);
    EXPECT_EQ(otherConn.getSubscriberRefCount().count, 0);

    incrementSignal.invoke(1);
    EXPECT_EQ(testValue, 2);

    otherConn = conn;
    EXPECT_TRUE(conn.getSubscriberRefCount().connected);
    EXPECT_EQ(conn.getSubscriberRefCount().count, 2);
    EXPECT_TRUE(otherConn.getSubscriberRefCount().connected);
    EXPECT_EQ(otherConn.getSubscriberRefCount().count, 2);

    // Signal should still work
    incrementSignal.invoke(1);
    EXPECT_EQ(testValue, 3);

    // Disconnect of moved-away connection should return false and not affect the signal
    EXPECT_TRUE(conn.disconnect());
    EXPECT_FALSE(conn.getSubscriberRefCount().connected);
    EXPECT_EQ(conn.getSubscriberRefCount().count, 0);
    EXPECT_TRUE(otherConn.getSubscriberRefCount().connected);
    EXPECT_EQ(otherConn.getSubscriberRefCount().count, 1);
    incrementSignal.invoke(1);
    EXPECT_EQ(testValue, 4);

    // Disconnect of moved-to connection should return true and affect the signal
    EXPECT_TRUE(otherConn.disconnect());
    incrementSignal.invoke(1);
    EXPECT_EQ(testValue, 4);
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

TEST(Connection, AssignmentOperatorMoveOther)
{
    Signal<int> incrementSignal;
    int testValue = 0;
    auto IncrementA = [&testValue](int incrementBy) {
        testValue += incrementBy;  //
    };
    EXPECT_EQ(testValue, 0);

    auto conn = incrementSignal.connect(IncrementA);

    EXPECT_TRUE(conn.getSubscriberRefCount().connected);
    EXPECT_EQ(conn.getSubscriberRefCount().count, 1);

    incrementSignal.invoke(1);
    EXPECT_EQ(testValue, 1);

    // Test copying of connection
    Connection otherConn;
    EXPECT_FALSE(otherConn.getSubscriberRefCount().connected);
    EXPECT_EQ(otherConn.getSubscriberRefCount().count, 0);

    incrementSignal.invoke(1);
    EXPECT_EQ(testValue, 2);

    otherConn = std::move(conn);
    EXPECT_FALSE(conn.getSubscriberRefCount().connected);
    EXPECT_EQ(conn.getSubscriberRefCount().count, 0);
    EXPECT_TRUE(otherConn.getSubscriberRefCount().connected);
    EXPECT_EQ(otherConn.getSubscriberRefCount().count, 1);

    // Test moving to existing connection
    Connection anotherConn = incrementSignal.connect(IncrementA);
    EXPECT_TRUE(anotherConn.getSubscriberRefCount().connected);
    EXPECT_EQ(anotherConn.getSubscriberRefCount().count, 1);

    incrementSignal.invoke(1);
    EXPECT_EQ(testValue, 4);

    Connection otherConnBackup = otherConn;
    EXPECT_TRUE(otherConnBackup.getSubscriberRefCount().connected);
    EXPECT_EQ(otherConnBackup.getSubscriberRefCount().count, 2);
    otherConn = std::move(anotherConn);
    EXPECT_FALSE(anotherConn.getSubscriberRefCount().connected);
    EXPECT_EQ(anotherConn.getSubscriberRefCount().count, 0);
    EXPECT_TRUE(otherConn.getSubscriberRefCount().connected);
    EXPECT_EQ(otherConn.getSubscriberRefCount().count, 1);
    EXPECT_TRUE(otherConnBackup.getSubscriberRefCount().connected);
    EXPECT_EQ(otherConnBackup.getSubscriberRefCount().count, 1);

    // Signal should still work
    incrementSignal.invoke(1);
    EXPECT_EQ(testValue, 6);

    // Disconnect of moved-away connection should return false and not affect the signal
    EXPECT_FALSE(conn.disconnect());
    incrementSignal.invoke(1);
    EXPECT_EQ(testValue, 8);

    // Disconnect of (another) moved-to connection should return true and affect the signal
    EXPECT_TRUE(otherConn.disconnect());
    incrementSignal.invoke(1);
    EXPECT_EQ(testValue, 9);

    // Disconnect of copied connection should return true and affect the signal
    EXPECT_TRUE(otherConnBackup.disconnect());
    incrementSignal.invoke(1);
    EXPECT_EQ(testValue, 9);
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
