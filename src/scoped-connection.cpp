#include <pajlada/signals/connection.hpp>
#include <pajlada/signals/scoped-connection.hpp>
#include <pajlada/signals/signal.hpp>

#include <gtest/gtest.h>

#include <vector>

namespace pajlada::Signals {

// TODO(pajlada): Make tests that try to put connections/signals in various STL containers

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

        EXPECT_TRUE(scopedConn.connection.getSubscriberRefCount().connected);
        EXPECT_EQ(scopedConn.connection.getSubscriberRefCount().count, 1);

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

        EXPECT_TRUE(scopedConn.connection.getSubscriberRefCount().connected);
        EXPECT_EQ(scopedConn.connection.getSubscriberRefCount().count, 1);

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

        EXPECT_TRUE(scopedConn.connection.getSubscriberRefCount().connected);
        EXPECT_EQ(scopedConn.connection.getSubscriberRefCount().count, 2);

        incrementSignal.invoke(1);
        EXPECT_EQ(a, 1);

        EXPECT_TRUE(conn.disconnect());
    }

    incrementSignal.invoke(1);
    EXPECT_EQ(a, 1);
}

TEST(ScopedConnection, MoveConstructSelf)
{
    Signal<int> signal;
    int a = 0;
    auto cb = [&](int i) { a += i; };

    ScopedConnection conn1 = signal.connect(cb);

    signal.invoke(1);
    EXPECT_EQ(a, 1);

    ScopedConnection conn2(std::move(conn1));
    signal.invoke(1);
    EXPECT_EQ(a, 2);
    EXPECT_FALSE(conn1.connection.isConnected());
    EXPECT_TRUE(conn2.connection.isConnected());

    ScopedConnection conn3(std::move(conn1));
    signal.invoke(1);
    EXPECT_EQ(a, 3);
    EXPECT_FALSE(conn1.connection.isConnected());
    EXPECT_TRUE(conn2.connection.isConnected());
    EXPECT_FALSE(conn3.connection.isConnected());

    {
        ScopedConnection conn4(std::move(conn2));
        signal.invoke(1);
        EXPECT_EQ(a, 4);
        EXPECT_FALSE(conn1.connection.isConnected());
        EXPECT_FALSE(conn2.connection.isConnected());
        EXPECT_FALSE(conn3.connection.isConnected());
        EXPECT_TRUE(conn4.connection.isConnected());
    }

    signal.invoke(1);
    EXPECT_EQ(a, 4);
}

TEST(ScopedConnection, MoveAssignSelf)
{
    Signal<int> signal;
    int a = 0;
    int b = 0;
    auto cbA = [&](int i) { a += i; };
    auto cbB = [&](int i) { b += i; };

    ScopedConnection connA = signal.connect(cbA);
    ScopedConnection connB = signal.connect(cbB);

    signal.invoke(1);
    EXPECT_EQ(a, 1);
    EXPECT_EQ(b, 1);

    connA = std::move(connB);
    signal.invoke(1);
    EXPECT_EQ(a, 1);
    EXPECT_EQ(b, 2);
    EXPECT_TRUE(connA.connection.isConnected());
    EXPECT_FALSE(connB.connection.isConnected());

    connA = std::move(connA);  // NOLINT
    signal.invoke(1);
    EXPECT_EQ(a, 1);
    EXPECT_EQ(b, 3);
    EXPECT_TRUE(connA.connection.isConnected());
    EXPECT_FALSE(connB.connection.isConnected());

    connA = ScopedConnection{};
    signal.invoke(1);
    EXPECT_EQ(a, 1);
    EXPECT_EQ(b, 3);
    EXPECT_FALSE(connA.connection.isConnected());
    EXPECT_FALSE(connB.connection.isConnected());
}

TEST(ScopedConnection, MoveAssignConnection)
{
    Signal<int> signal;
    int a = 0;
    int b = 0;
    auto cbA = [&](int i) { a += i; };
    auto cbB = [&](int i) { b += i; };

    ScopedConnection connA = signal.connect(cbA);
    signal.invoke(1);
    EXPECT_EQ(a, 1);
    EXPECT_EQ(b, 0);

    connA = signal.connect(cbB);
    signal.invoke(1);
    EXPECT_EQ(a, 1);
    EXPECT_EQ(b, 1);

    EXPECT_EQ(connA.connection.getSubscriberRefCount().count, 1);
    Connection copy = connA.connection;
    EXPECT_EQ(connA.connection.getSubscriberRefCount().count, 2);
    EXPECT_EQ(copy.getSubscriberRefCount().count, 2);

    signal.invoke(1);
    EXPECT_EQ(a, 1);
    EXPECT_EQ(b, 2);

    connA = std::move(copy);
    EXPECT_EQ(connA.connection.getSubscriberRefCount().count, 1);

    signal.invoke(1);
    EXPECT_EQ(a, 1);
    EXPECT_EQ(b, 3);

    connA = ScopedConnection{};

    signal.invoke(1);
    EXPECT_EQ(a, 1);
    EXPECT_EQ(b, 3);
}

TEST(ScopedConnection, STLContainerUnique)
{
    Signal<int> incrementSignal;
    int a = 0;
    auto IncrementA = [&a](int incrementBy) {
        a += incrementBy;  //
    };
    EXPECT_EQ(a, 0);

    std::vector<std::unique_ptr<ScopedConnection>> scopedConnections;

    {
        auto scopedConn = std::make_unique<ScopedConnection>(
            incrementSignal.connect(IncrementA));

        EXPECT_TRUE(scopedConn->connection.getSubscriberRefCount().connected);
        EXPECT_EQ(scopedConn->connection.getSubscriberRefCount().count, 1);

        scopedConnections.emplace_back(std::move(scopedConn));

        EXPECT_TRUE(
            scopedConnections[0]->connection.getSubscriberRefCount().connected);
        EXPECT_EQ(
            scopedConnections[0]->connection.getSubscriberRefCount().count, 1);

        incrementSignal.invoke(1);
        EXPECT_EQ(a, 1);
    }

    incrementSignal.invoke(1);
    EXPECT_EQ(a, 2);

    scopedConnections.clear();

    incrementSignal.invoke(1);
    EXPECT_EQ(a, 2);
}

TEST(ScopedConnection, VectorUniqueEmplaceBack)
{
    Signal<int> incrementSignal;
    int a = 0;
    auto IncrementA = [&a](int incrementBy) {
        a += incrementBy;  //
    };
    EXPECT_EQ(a, 0);

    std::vector<std::unique_ptr<ScopedConnection>> scopedConnections;

    {
        scopedConnections.emplace_back(std::make_unique<ScopedConnection>(
            incrementSignal.connect(IncrementA)));

        incrementSignal.invoke(1);
        EXPECT_EQ(a, 1);
    }

    incrementSignal.invoke(1);
    EXPECT_EQ(a, 2);

    scopedConnections.clear();

    incrementSignal.invoke(1);
    EXPECT_EQ(a, 2);
}

TEST(ScopedConnection, STLContainer)
{
    Signal<int> signal;
    int a = 0;
    auto cb = [&](int incrementBy) { a += incrementBy; };

    std::vector<ScopedConnection> connections;

    {
        ScopedConnection scopedConn = signal.connect(cb);

        EXPECT_TRUE(scopedConn.connection.getSubscriberRefCount().connected);
        EXPECT_EQ(scopedConn.connection.getSubscriberRefCount().count, 1);

        connections.emplace_back(std::move(scopedConn));

        EXPECT_TRUE(
            connections[0].connection.getSubscriberRefCount().connected);
        EXPECT_EQ(connections[0].connection.getSubscriberRefCount().count, 1);

        signal.invoke(1);
        EXPECT_EQ(a, 1);
    }

    signal.invoke(1);
    EXPECT_EQ(a, 2);

    connections.clear();

    signal.invoke(1);
    EXPECT_EQ(a, 2);
}

TEST(ScopedConnection, VectorEmplaceBack)
{
    Signal<int> signal;
    int a = 0;
    auto cb = [&a](int incrementBy) { a += incrementBy; };

    std::vector<ScopedConnection> connections;

    {
        connections.emplace_back(signal.connect(cb));

        signal.invoke(1);
        EXPECT_EQ(a, 1);
    }

    signal.invoke(1);
    EXPECT_EQ(a, 2);

    connections.clear();

    signal.invoke(1);
    EXPECT_EQ(a, 2);
}

}  // namespace pajlada::Signals
