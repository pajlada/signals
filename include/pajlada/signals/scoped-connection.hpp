#pragma once

#include "pajlada/signals/connection.hpp"

#include <utility>

#if __has_include(<gtest/gtest_prod.h>)
#include <gtest/gtest_prod.h>
#endif

namespace pajlada {
namespace Signals {

class ScopedConnection
{
    Connection connection;

#if __has_include(<gtest/gtest_prod.h>)
    FRIEND_TEST(ScopedConnection, MoveAssignConnection);
#endif

public:
    ScopedConnection() = default;

    ScopedConnection(ScopedConnection &&other) noexcept
        : connection(std::move(other.connection))
    {
    }

    ScopedConnection(Connection &&_connection) noexcept
        : connection(std::move(_connection))
    {
    }

    // Copying a connection may have dangerous unseen side-effects, therefore they may not happen unless explicitly converted
    explicit ScopedConnection(const Connection &other)
        : connection(other)
    {
    }

    ScopedConnection(const ScopedConnection &other) = delete;
    ScopedConnection &operator=(const Connection &other) = delete;
    ScopedConnection &operator=(const ScopedConnection &other) = delete;

    ScopedConnection &
    operator=(ScopedConnection &&other)
    {
        if (&other == this) {
            return *this;
        }

        this->connection.disconnect();
        this->connection = std::move(other.connection);
        return *this;
    }

    ScopedConnection &
    operator=(Connection &&other)
    {
        this->connection.disconnect();
        this->connection = std::move(other);
        return *this;
    }

    ~ScopedConnection()
    {
        this->connection.disconnect();
    }

    [[nodiscard]]
    Connection::SubscriberRefCountResponse
    getSubscriberRefCount() const
    {
        return this->connection.getSubscriberRefCount();
    }

    [[nodiscard]]
    bool
    isConnected() const
    {
        return this->connection.isConnected();
    }

    bool
    block()
    {
        return this->connection.block();
    }

    bool
    unblock()
    {
        return this->connection.unblock();
    }

    [[nodiscard]]
    bool
    isBlocked() const
    {
        return this->connection.isBlocked();
    }
};

}  // namespace Signals
}  // namespace pajlada
