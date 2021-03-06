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

TEST(ScopedConnection, ConstructFromImplicitlyMovedConnection)
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
        ScopedConnection scopedConn(incrementSignal.connect(IncrementA));

        EXPECT_TRUE(scopedConn.c().getSubscriberRefCount().connected);
        EXPECT_EQ(scopedConn.c().getSubscriberRefCount().count, 1);

        incrementSignal.invoke(1);
        EXPECT_EQ(a, 1);
    }

    incrementSignal.invoke(1);
    EXPECT_EQ(a, 1);
}

TEST(ScopedConnection, ConstructFromCopiedConnection)
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

TEST(ScopedConnection, ConstructFromCopiedScopedConnection)
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

TEST(ScopedConnection, VectorPushBack)
{
    Signal<int> incrementSignal;
    int a = 0;
    auto IncrementA = [&a](int incrementBy) {
        a += incrementBy;  //
    };
    EXPECT_EQ(a, 0);

    vector<ScopedConnection> scopedConnections;

    {
        scopedConnections.push_back(incrementSignal.connect(IncrementA));

        incrementSignal.invoke(1);
        EXPECT_EQ(a, 1);
    }

    incrementSignal.invoke(1);
    EXPECT_EQ(a, 2);

    scopedConnections.clear();

    incrementSignal.invoke(1);
    EXPECT_EQ(a, 2);
}

TEST(ScopedConnection, VectorEmplaceBack)
{
    Signal<int> incrementSignal;
    int a = 0;
    auto IncrementA = [&a](int incrementBy) {
        a += incrementBy;  //
    };
    EXPECT_EQ(a, 0);

    vector<ScopedConnection> scopedConnections;

    {
        scopedConnections.emplace_back(incrementSignal.connect(IncrementA));

        incrementSignal.invoke(1);
        EXPECT_EQ(a, 1);
    }

    incrementSignal.invoke(1);
    EXPECT_EQ(a, 2);

    scopedConnections.clear();

    incrementSignal.invoke(1);
    EXPECT_EQ(a, 2);
}

TEST(ScopedConnection, AssignmentOperatorCopyConnection)
{
    Signal<int> incrementSignal;
    int a = 0;
    auto IncrementA = [&a](int incrementBy) {
        a += incrementBy;  //
    };
    EXPECT_EQ(a, 0);

    // Nothing connected, should not change
    incrementSignal.invoke(1);
    EXPECT_EQ(a, 0);

    {
        Connection connA = incrementSignal.connect(IncrementA);

        EXPECT_TRUE(connA.getSubscriberRefCount().connected);
        EXPECT_EQ(connA.getSubscriberRefCount().count, 1);

        Connection connB = incrementSignal.connect(IncrementA);

        EXPECT_TRUE(connB.getSubscriberRefCount().connected);
        EXPECT_EQ(connB.getSubscriberRefCount().count, 1);

        // 2 connections connected
        incrementSignal.invoke(1);
        EXPECT_EQ(a, 2);

        {
            ScopedConnection scopedConn(connA);

            EXPECT_TRUE(connA.getSubscriberRefCount().connected);
            EXPECT_EQ(connA.getSubscriberRefCount().count, 2);
            EXPECT_TRUE(connB.getSubscriberRefCount().connected);
            EXPECT_EQ(connB.getSubscriberRefCount().count, 1);

            scopedConn = connB;

            EXPECT_TRUE(connA.getSubscriberRefCount().connected);
            EXPECT_EQ(connA.getSubscriberRefCount().count, 1);
            EXPECT_TRUE(scopedConn.c().getSubscriberRefCount().connected);
            EXPECT_EQ(scopedConn.c().getSubscriberRefCount().count, 2);

            scopedConn = connA;

            EXPECT_TRUE(connA.getSubscriberRefCount().connected);
            EXPECT_EQ(connA.getSubscriberRefCount().count, 2);
            EXPECT_TRUE(connB.getSubscriberRefCount().connected);
            EXPECT_EQ(connB.getSubscriberRefCount().count, 1);

            // 2 connections still connected
            incrementSignal.invoke(1);
            EXPECT_EQ(a, 4);
        }

        EXPECT_TRUE(connA.getSubscriberRefCount().connected);
        EXPECT_EQ(connA.getSubscriberRefCount().count, 1);
        EXPECT_TRUE(connB.getSubscriberRefCount().connected);
        EXPECT_EQ(connB.getSubscriberRefCount().count, 1);
    }
}

TEST(ScopedConnection, AssignmentOperatorCopyScopedConnection)
{
    Signal<int> incrementSignal;
    int a = 0;
    auto IncrementA = [&a](int incrementBy) {
        a += incrementBy;  //
    };
    EXPECT_EQ(a, 0);

    // Nothing connected, should not change
    incrementSignal.invoke(1);
    EXPECT_EQ(a, 0);

    {
        Connection connA = incrementSignal.connect(IncrementA);

        EXPECT_TRUE(connA.getSubscriberRefCount().connected);
        EXPECT_EQ(connA.getSubscriberRefCount().count, 1);

        Connection connB = incrementSignal.connect(IncrementA);

        EXPECT_TRUE(connB.getSubscriberRefCount().connected);
        EXPECT_EQ(connB.getSubscriberRefCount().count, 1);

        // 2 connections connected
        incrementSignal.invoke(1);
        EXPECT_EQ(a, 2);

        {
            ScopedConnection scopedConn(connA);
            ScopedConnection scB(connB);

            EXPECT_TRUE(connA.getSubscriberRefCount().connected);
            EXPECT_EQ(connA.getSubscriberRefCount().count, 2);
            EXPECT_TRUE(connB.getSubscriberRefCount().connected);
            EXPECT_EQ(connB.getSubscriberRefCount().count, 2);

            scopedConn = scB;

            EXPECT_TRUE(connA.getSubscriberRefCount().connected);
            EXPECT_EQ(connA.getSubscriberRefCount().count, 1);
            EXPECT_TRUE(connB.getSubscriberRefCount().connected);
            EXPECT_EQ(connB.getSubscriberRefCount().count, 3);

            // 2 connections still connected
            incrementSignal.invoke(1);
            EXPECT_EQ(a, 4);
        }

        EXPECT_TRUE(connA.getSubscriberRefCount().connected);
        EXPECT_EQ(connA.getSubscriberRefCount().count, 1);
        EXPECT_TRUE(connB.getSubscriberRefCount().connected);
        EXPECT_EQ(connB.getSubscriberRefCount().count, 1);
    }
}

TEST(ScopedConnection, AssignmentOperatorMoveScopedConnection)
{
    Signal<int> incrementSignal;
    int a = 0;
    auto IncrementA = [&a](int incrementBy) {
        a += incrementBy;  //
    };
    EXPECT_EQ(a, 0);

    // Nothing connected, should not change
    incrementSignal.invoke(1);
    EXPECT_EQ(a, 0);

    {
        Connection connA = incrementSignal.connect(IncrementA);

        EXPECT_TRUE(connA.getSubscriberRefCount().connected);
        EXPECT_EQ(connA.getSubscriberRefCount().count, 1);

        Connection connB = incrementSignal.connect(IncrementA);

        EXPECT_TRUE(connB.getSubscriberRefCount().connected);
        EXPECT_EQ(connB.getSubscriberRefCount().count, 1);

        // 2 connections connected
        incrementSignal.invoke(1);
        EXPECT_EQ(a, 2);

        {
            ScopedConnection scopedConn(connA);
            ScopedConnection scB(connB);

            EXPECT_TRUE(connA.getSubscriberRefCount().connected);
            EXPECT_EQ(connA.getSubscriberRefCount().count, 2);
            EXPECT_TRUE(connB.getSubscriberRefCount().connected);
            EXPECT_EQ(connB.getSubscriberRefCount().count, 2);

            scopedConn = std::move(scB);

            EXPECT_TRUE(connA.getSubscriberRefCount().connected);
            EXPECT_EQ(connA.getSubscriberRefCount().count, 1);
            EXPECT_TRUE(connB.getSubscriberRefCount().connected);
            EXPECT_EQ(connB.getSubscriberRefCount().count, 2);

            // 2 connections still connected
            incrementSignal.invoke(1);
            EXPECT_EQ(a, 4);
        }

        EXPECT_TRUE(connA.getSubscriberRefCount().connected);
        EXPECT_EQ(connA.getSubscriberRefCount().count, 1);
        EXPECT_TRUE(connB.getSubscriberRefCount().connected);
        EXPECT_EQ(connB.getSubscriberRefCount().count, 1);
    }
}

TEST(ScopedConnection, AssignmentOperatorMoveScopedConnectionSelf)
{
    Signal<int> incrementSignal;
    int a = 0;
    auto IncrementA = [&a](int incrementBy) {
        a += incrementBy;  //
    };
    EXPECT_EQ(a, 0);

    // Nothing connected, should not change
    incrementSignal.invoke(1);
    EXPECT_EQ(a, 0);

    {
        Connection connA = incrementSignal.connect(IncrementA);

        EXPECT_TRUE(connA.getSubscriberRefCount().connected);
        EXPECT_EQ(connA.getSubscriberRefCount().count, 1);

        Connection connB = incrementSignal.connect(IncrementA);

        EXPECT_TRUE(connB.getSubscriberRefCount().connected);
        EXPECT_EQ(connB.getSubscriberRefCount().count, 1);

        // 2 connections connected
        incrementSignal.invoke(1);
        EXPECT_EQ(a, 2);

        {
            ScopedConnection scopedConn(connA);

            EXPECT_TRUE(connA.getSubscriberRefCount().connected);
            EXPECT_EQ(connA.getSubscriberRefCount().count, 2);
            EXPECT_TRUE(connB.getSubscriberRefCount().connected);
            EXPECT_EQ(connB.getSubscriberRefCount().count, 1);

            scopedConn = std::move(scopedConn);

            EXPECT_TRUE(connA.getSubscriberRefCount().connected);
            EXPECT_EQ(connA.getSubscriberRefCount().count, 2);
            EXPECT_TRUE(connB.getSubscriberRefCount().connected);
            EXPECT_EQ(connB.getSubscriberRefCount().count, 1);

            // 2 connections still connected
            incrementSignal.invoke(1);
            EXPECT_EQ(a, 4);
        }

        EXPECT_TRUE(connA.getSubscriberRefCount().connected);
        EXPECT_EQ(connA.getSubscriberRefCount().count, 1);
        EXPECT_TRUE(connB.getSubscriberRefCount().connected);
        EXPECT_EQ(connB.getSubscriberRefCount().count, 1);
    }
}

TEST(ScopedConnection, AssignmentOperatorMove)
{
    Signal<int> incrementSignal;
    int a = 0;
    auto IncrementA = [&a](int incrementBy) {
        a += incrementBy;  //
    };
    EXPECT_EQ(a, 0);

    // Nothing connected, should not change
    incrementSignal.invoke(1);
    EXPECT_EQ(a, 0);

    {
        Connection connA = incrementSignal.connect(IncrementA);

        EXPECT_TRUE(connA.getSubscriberRefCount().connected);
        EXPECT_EQ(connA.getSubscriberRefCount().count, 1);

        Connection connB = incrementSignal.connect(IncrementA);

        EXPECT_TRUE(connB.getSubscriberRefCount().connected);
        EXPECT_EQ(connB.getSubscriberRefCount().count, 1);

        // 2 connections connected
        incrementSignal.invoke(1);
        EXPECT_EQ(a, 2);

        {
            ScopedConnection scopedConn(connA);

            EXPECT_TRUE(connA.getSubscriberRefCount().connected);
            EXPECT_EQ(connA.getSubscriberRefCount().count, 2);
            EXPECT_TRUE(connB.getSubscriberRefCount().connected);
            EXPECT_EQ(connB.getSubscriberRefCount().count, 1);

            scopedConn = std::move(connB);

            // 2 connections still connected
            incrementSignal.invoke(1);
            EXPECT_EQ(a, 4);

            EXPECT_TRUE(connA.getSubscriberRefCount().connected);
            EXPECT_EQ(connA.getSubscriberRefCount().count, 1);
            EXPECT_FALSE(connB.getSubscriberRefCount().connected);
            EXPECT_EQ(connB.getSubscriberRefCount().count, 0);
            EXPECT_TRUE(scopedConn.c().getSubscriberRefCount().connected);
            EXPECT_EQ(scopedConn.c().getSubscriberRefCount().count, 1);

            scopedConn = std::move(connA);

            // connB is gone now, so only 1 should be connected
            incrementSignal.invoke(1);
            EXPECT_EQ(a, 5);

            EXPECT_FALSE(connA.getSubscriberRefCount().connected);
            EXPECT_EQ(connA.getSubscriberRefCount().count, 0);
            EXPECT_FALSE(connB.getSubscriberRefCount().connected);
            EXPECT_EQ(connB.getSubscriberRefCount().count, 0);

            EXPECT_TRUE(scopedConn.c().getSubscriberRefCount().connected);
            EXPECT_EQ(scopedConn.c().getSubscriberRefCount().count, 1);
        }

        // connB and connA are gone now, since connA was moved into the scoped connection
        // so nothing should change
        incrementSignal.invoke(1);
        EXPECT_EQ(a, 5);

        EXPECT_FALSE(connA.getSubscriberRefCount().connected);
        EXPECT_EQ(connA.getSubscriberRefCount().count, 0);
        EXPECT_FALSE(connB.getSubscriberRefCount().connected);
        EXPECT_EQ(connB.getSubscriberRefCount().count, 0);
    }
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
