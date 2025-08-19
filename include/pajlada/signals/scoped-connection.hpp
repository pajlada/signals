#pragma once

#include "pajlada/signals/connection.hpp"

#if __has_include(<gtest/gtest_prod.h>)
#include <gtest/gtest_prod.h>
#endif

namespace pajlada {
namespace Signals {

// A ScopedConnection should always be stored in a unique_ptr to avoid unneccesary moves and copies
class ScopedConnection
{
    Connection connection;

#if __has_include(<gtest/gtest_prod.h>)
    FRIEND_TEST(ScopedConnection, MoveConstructorFromBase);
    FRIEND_TEST(ScopedConnection, ConstructFromImplicitlyMovedConnection);
    FRIEND_TEST(ScopedConnection, ConstructFromCopiedConnection);
    FRIEND_TEST(ScopedConnection, STLContainer);
#endif

public:
    ScopedConnection() = delete;

    ScopedConnection(ScopedConnection &&other) = delete;

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

    ScopedConnection &operator=(ScopedConnection &&other) = delete;

    ScopedConnection &operator=(Connection &&other) = delete;

    ~ScopedConnection()
    {
        this->connection.disconnect();
    }
};

}  // namespace Signals
}  // namespace pajlada
