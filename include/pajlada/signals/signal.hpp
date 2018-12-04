#pragma once

#include "pajlada/signals/connection.hpp"

#include <atomic>
#include <functional>
#include <memory>
#include <vector>

namespace pajlada {
namespace Signals {

template <typename... Args>
class Signal
{
public:
    Signal()
        : latestConnection(0)
    {
    }

    using CallbackBodyType = detail::CallbackBody<Args...>;

    Connection
    connect(typename CallbackBodyType::FunctionSignature func)
    {
        uint64_t connectionIndex = this->nextConnection();

        auto callback = std::make_shared<CallbackBodyType>(connectionIndex);
        callback->func = std::move(func);

        std::weak_ptr<CallbackBodyType> weakCallback(callback);

        this->callbackBodies.emplace_back(std::move(callback));

        return Connection(weakCallback);
    }

    void
    invoke(Args... args)
    {
        for (auto it = this->callbackBodies.begin();
             it != this->callbackBodies.end();) {
            auto &callback = *it;

            if (!callback->isConnected()) {
                // Clean up disconnected callbacks
                it = this->callbackBodies.erase(it);
                continue;
            }

            if (!callback->isBlocked()) {
                callback->func(std::forward<Args>(args)...);
            }

            ++it;
        }
    }

    void
    disconnectAll()
    {
        for (auto &&body : this->callbackBodies) {
            body->disconnect();
        }
    }

private:
    std::atomic<uint64_t> latestConnection;
    std::vector<std::shared_ptr<CallbackBodyType>> callbackBodies;

    uint64_t
    nextConnection()
    {
        return ++this->latestConnection;
    }
};

using NoArgSignal = Signal<>;

/// Bolt Signals (1-time use)
// connect is fast
// disconnect doesn't exist
// invoke is fast
template <class... Args>
class BoltSignal
{
protected:
    typedef std::function<void(Args...)> CallbackType;

public:
    void
    connect(CallbackType cb)
    {
        this->callbacks.push_back(std::move(cb));
    }

    void
    invoke(Args... args)
    {
        for (auto &callback : this->callbacks) {
            callback.func(std::forward<Args>(args)...);
        }

        this->callbacks.clear();
    }

protected:
    std::vector<CallbackType> callbacks;
};

using NoArgBoltSignal = BoltSignal<>;

}  // namespace Signals
}  // namespace pajlada
