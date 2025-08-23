#pragma once

#include <pajlada/signals/connection.hpp>
#include <pajlada/signals/scoped-connection.hpp>

#include <vector>

namespace pajlada {
namespace Signals {

class SignalHolder
{
    std::vector<ScopedConnection> _managedConnections;

    void
    add(ScopedConnection &&connection)
    {
        this->_managedConnections.emplace_back(std::move(connection));
    }

public:
    SignalHolder() = default;
    ~SignalHolder() = default;

    SignalHolder(SignalHolder &&) = default;
    SignalHolder &operator=(SignalHolder &&) = default;
    SignalHolder(const SignalHolder &other) = delete;
    SignalHolder &operator=(const SignalHolder &other) = delete;

    void
    addConnection(ScopedConnection &&connection)
    {
        this->add(std::move(connection));
    }

    template <typename Signal, typename Callback>
    void
    managedConnect(Signal &signal, Callback cb)
    {
        this->add(signal.connect(std::forward<Callback>(cb)));
    }

    // Clear all connections held by this SignalHolder
    void
    clear()
    {
        this->_managedConnections.clear();
    }
};

}  // namespace Signals
}  // namespace pajlada
