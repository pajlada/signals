#pragma once

#include "pajlada/signals/connection.hpp"

namespace pajlada {
namespace Signals {

// A ScopedConnection should always be stored in a unique_ptr to avoid unneccesary moves and copies
class ScopedConnection
{
    Connection connection;

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

    // Copying a connection may have dangerous unseen side-effects, therefore they may not happen unless explicitly converted
    explicit ScopedConnection(const ScopedConnection &other)
        : connection(other.connection)
    {
    }

    ScopedConnection &
    operator=(const Connection &other)
    {
        this->connection = other;

        return *this;
    }

    ScopedConnection &
    operator=(const ScopedConnection &other)
    {
        this->connection = other.connection;

        return *this;
    }

    ScopedConnection &
    operator=(ScopedConnection &&other) noexcept
    {
        if (&other == this) {
            return *this;
        }

        this->connection = std::move(other.connection);

        return *this;
    }

    ScopedConnection &
    operator=(Connection &&other) noexcept
    {
        this->connection = std::move(other);

        return *this;
    }

    ~ScopedConnection()
    {
        this->connection.disconnect();
    }

#ifdef PAJLADA_TESTING
    // used for testing
    Connection &
    c()
    {
        return this->connection;
    }
#endif
};

}  // namespace Signals
}  // namespace pajlada
