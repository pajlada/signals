#pragma once

#include <pajlada/signals/connection.hpp>

#include <vector>

namespace pajlada {
namespace Signals {

class SignalHolder
{
    std::vector<ScopedConnection> _managedConnections;

public:
    virtual ~SignalHolder() = default;
    SignalHolder() = default;

    // Make non-copyable. Not sure if this is required
    SignalHolder(const SignalHolder &other) = delete;
    SignalHolder &operator=(const SignalHolder &other) = delete;

    void
    addConnection(Connection &&connection)
    {
        this->_managedConnections.emplace_back(connection);
    }

    template <typename Signal, typename Callback>
    void
    managedConnect(Signal &signal, Callback cb)
    {
        this->_managedConnections.emplace_back(signal.connect(cb));
    }
};

}  // namespace Signals
}  // namespace pajlada
