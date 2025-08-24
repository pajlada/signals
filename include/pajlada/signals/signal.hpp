#pragma once

#include "pajlada/signals/connection.hpp"

#include <algorithm>
#include <functional>
#include <memory>
#include <mutex>
#include <vector>

namespace pajlada {
namespace Signals {

template <typename... Args>
class Signal
{
public:
    using CallbackBodyType = detail::CallbackBody<Args...>;

    [[nodiscard]] Connection
    connect(typename CallbackBodyType::FunctionSignature func)
    {
        auto callback = std::make_shared<CallbackBodyType>();
        callback->func = std::move(func);

        std::weak_ptr<CallbackBodyType> weakCallback(callback);

        this->registerBody(std::move(callback));

        return Connection(weakCallback);
    }

    void
    invoke(Args... args)
    {
        auto activeBodies = this->getActiveBodies();

        for (const auto &cb : activeBodies) {
            cb->func(std::forward<Args>(args)...);
        }
    }

private:
    std::mutex callbackBodiesMutex;
    std::vector<std::shared_ptr<CallbackBodyType>> callbackBodies;

    std::vector<std::shared_ptr<CallbackBodyType>>
    getActiveBodies()
    {
        std::vector<std::shared_ptr<CallbackBodyType>> activeBodies;

        std::unique_lock<std::mutex> lock(this->callbackBodiesMutex);

        for (auto it = this->callbackBodies.begin();
             it != this->callbackBodies.end();) {
            auto &callback = *it;

            if (!callback->isConnected()) {
                // Clean up disconnected callbacks
                it = this->callbackBodies.erase(it);
                continue;
            }

            if (!callback->isBlocked()) {
                activeBodies.emplace_back(callback);
            }

            ++it;
        }

        return activeBodies;
    }

    void
    registerBody(std::shared_ptr<CallbackBodyType> &&body)
    {
        std::unique_lock<std::mutex> lock(this->callbackBodiesMutex);

        this->callbackBodies.emplace_back(std::move(body));
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

/// Disconnects callback when callback is true
template <class... Args>
class SelfDisconnectingSignal
{
protected:
    typedef std::function<bool(Args...)> CallbackType;

public:
    void
    connect(CallbackType cb)
    {
        this->callbacks.push_back(std::move(cb));
    }

    void
    invoke(Args... args)
    {
        callbacks.erase(
            std::remove_if(callbacks.begin(), callbacks.end(),
                           [&](CallbackType callback) {
                               return callback(std::forward<Args>(args)...);
                           }),
            callbacks.end());
    }

protected:
    std::vector<CallbackType> callbacks;
};

using NoArgSelfDisconnectingSignal = SelfDisconnectingSignal<>;

}  // namespace Signals
}  // namespace pajlada
