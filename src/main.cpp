#include <gtest/gtest.h>
#include <pajlada/signals/signal.hpp>

#include <vector>

using namespace pajlada::Signals;
using namespace std;

// TODO(pajlada): Make tests that try to put connections/signals in various STL containers

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

    incrementSignal.connect(IncrementA);

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

    incrementSignal.connect(IncrementA);
    incrementSignal.connect(IncrementA);

    incrementSignal.invoke(1);

    EXPECT_EQ(a, 2);

    incrementSignal.invoke(2);

    EXPECT_EQ(a, 6);
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

TEST(ScopedConnection, MoveConstructorFromBase)
{
    Signal<int> incrementSignal;
    int a = 0;
    auto IncrementA = [&a](int incrementBy) {
        a += incrementBy;  //
    };
    EXPECT_EQ(a, 0);

    {
        ScopedConnection scopedConn(incrementSignal.connect(IncrementA));

        EXPECT_TRUE(scopedConn.c().getSubscriberRefCount().connected);
        EXPECT_EQ(scopedConn.c().getSubscriberRefCount().count, 1);

        incrementSignal.invoke(1);
        EXPECT_EQ(a, 1);
    }

    incrementSignal.invoke(1);
    EXPECT_EQ(a, 1);
}

TEST(ScopedConnection, MoveAssignmentOperatorFromBase)
{
    Signal<int> incrementSignal;
    int a = 0;
    auto IncrementA = [&a](int incrementBy) {
        a += incrementBy;  //
    };
    EXPECT_EQ(a, 0);

    incrementSignal.invoke(1);

    EXPECT_EQ(a, 0);

    {
        ScopedConnection scopedConn = incrementSignal.connect(IncrementA);

        EXPECT_TRUE(scopedConn.c().getSubscriberRefCount().connected);
        EXPECT_EQ(scopedConn.c().getSubscriberRefCount().count, 1);

        incrementSignal.invoke(1);
        EXPECT_EQ(a, 1);
    }

    incrementSignal.invoke(1);
    EXPECT_EQ(a, 1);
}

TEST(ScopedConnection, CopyConstructorFromBase)
{
    Signal<int> incrementSignal;
    int a = 0;
    auto IncrementA = [&a](int incrementBy) {
        a += incrementBy;  //
    };
    EXPECT_EQ(a, 0);

    {
        auto conn = incrementSignal.connect(IncrementA);

        EXPECT_TRUE(conn.getSubscriberRefCount().connected);
        EXPECT_EQ(conn.getSubscriberRefCount().count, 1);

        ScopedConnection scopedConn(conn);

        EXPECT_TRUE(conn.getSubscriberRefCount().connected);
        EXPECT_EQ(conn.getSubscriberRefCount().count, 2);

        EXPECT_TRUE(scopedConn.c().getSubscriberRefCount().connected);
        EXPECT_EQ(scopedConn.c().getSubscriberRefCount().count, 2);

        incrementSignal.invoke(1);
        EXPECT_EQ(a, 1);

        EXPECT_TRUE(conn.disconnect());
    }

    incrementSignal.invoke(1);
    EXPECT_EQ(a, 1);
}

TEST(ScopedConnection, CopyConstructorFromScopedConnection)
{
    Signal<int> incrementSignal;
    int a = 0;
    auto IncrementA = [&a](int incrementBy) {
        a += incrementBy;  //
    };
    EXPECT_EQ(a, 0);

    {
        ScopedConnection scopedConn(incrementSignal.connect(IncrementA));

        EXPECT_TRUE(scopedConn.c().getSubscriberRefCount().connected);
        EXPECT_EQ(scopedConn.c().getSubscriberRefCount().count, 1);

        ScopedConnection scopedConnCopy(scopedConn);

        EXPECT_TRUE(scopedConn.c().getSubscriberRefCount().connected);
        EXPECT_EQ(scopedConn.c().getSubscriberRefCount().count, 2);

        EXPECT_TRUE(scopedConnCopy.c().getSubscriberRefCount().connected);
        EXPECT_EQ(scopedConnCopy.c().getSubscriberRefCount().count, 2);

        incrementSignal.invoke(1);
        EXPECT_EQ(a, 1);
    }

    incrementSignal.invoke(1);
    EXPECT_EQ(a, 1);
}

TEST(ScopedConnection, STLContainer)
{
    Signal<int> incrementSignal;
    int a = 0;
    auto IncrementA = [&a](int incrementBy) {
        a += incrementBy;  //
    };
    EXPECT_EQ(a, 0);

    vector<ScopedConnection> scopedConnections;

    {
        ScopedConnection scopedConn(incrementSignal.connect(IncrementA));

        EXPECT_TRUE(scopedConn.c().getSubscriberRefCount().connected);
        EXPECT_EQ(scopedConn.c().getSubscriberRefCount().count, 1);

        ScopedConnection scopedConnCopy(scopedConn);

        EXPECT_TRUE(scopedConn.c().getSubscriberRefCount().connected);
        EXPECT_EQ(scopedConn.c().getSubscriberRefCount().count, 2);

        ScopedConnection scopedConnMoved(move(scopedConn));

        EXPECT_TRUE(scopedConnMoved.c().getSubscriberRefCount().connected);
        EXPECT_EQ(scopedConnMoved.c().getSubscriberRefCount().count, 2);

        EXPECT_TRUE(scopedConnCopy.c().getSubscriberRefCount().connected);
        EXPECT_EQ(scopedConnCopy.c().getSubscriberRefCount().count, 2);

        scopedConnections.emplace_back(move(scopedConnCopy));

        EXPECT_TRUE(scopedConnMoved.c().getSubscriberRefCount().connected);
        EXPECT_EQ(scopedConnMoved.c().getSubscriberRefCount().count, 2);

        scopedConnections.emplace_back(move(scopedConnMoved));

        EXPECT_TRUE(
            scopedConnections.front().c().getSubscriberRefCount().connected);
        EXPECT_EQ(scopedConnections.front().c().getSubscriberRefCount().count,
                  2);

        incrementSignal.invoke(1);
        EXPECT_EQ(a, 1);
    }

    incrementSignal.invoke(1);
    EXPECT_EQ(a, 2);

    scopedConnections.clear();

    incrementSignal.invoke(1);
    EXPECT_EQ(a, 2);
}

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
