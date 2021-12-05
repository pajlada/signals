#pragma once

#include <pajlada/signals/connection.hpp>
#include <pajlada/signals/scoped-connection.hpp>

#include <memory>
#include <vector>

namespace pajlada {
namespace Signals {

class SignalHolder
{
    std::vector<std::unique_ptr<ScopedConnection>> _managedConnections;

    void
    add(Connection &&connection)
    {
        auto scopedConnection =
            std::make_unique<ScopedConnection>(std::move(connection));

        this->add(std::move(scopedConnection));
    }

    void
    add(std::unique_ptr<ScopedConnection> &&scopedConnection)
    {
        this->_managedConnections.emplace_back(std::move(scopedConnection));
    }

public:
    virtual ~SignalHolder() = default;
    SignalHolder() = default;

    // Make non-copyable. Not sure if this is required
    SignalHolder(const SignalHolder &other) = delete;
    SignalHolder &operator=(const SignalHolder &other) = delete;

    void
    addConnection(Connection &&connection)
    {
        this->add(std::move(connection));
    }

    template <typename Signal, typename Callback>
    void
    managedConnect(Signal &signal, Callback cb)
    {
        this->add(std::move(signal.connect(cb)));
    }

    // Clear all connections held by this SignalHolder
    void
    clear()
    {
        this->_managedConnections.clear();
    }

    // Ensure this can be used as a UserManagedConnectionManager in pajlada/settings
    void
    emplace_back(std::unique_ptr<ScopedConnection> &&scopedConnection)
    {
        this->add(std::move(scopedConnection));
    }
};

}  // namespace Signals
}  // namespace pajlada
