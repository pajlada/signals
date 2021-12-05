#pragma once

#include "pajlada/signals/connection.hpp"

namespace pajlada {
namespace Signals {

// A ScopedConnection should always be stored in a unique_ptr to avoid unneccesary moves and copies
class ScopedConnection
{
    Connection connection;

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
