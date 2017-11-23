#pragma once

#include "pajlada/signals/connection.hpp"

#include <atomic>
#include <functional>
#include <memory>
#include <vector>

namespace pajlada {
namespace Signals {
namespace detail {

template <typename... Args>
class Subscriber
{
};

template <typename... Args>
class BaseSignal
{
protected:
    BaseSignal()
        : latestConnection(0)
    {
    }

public:
    using CallbackBodyType = CallbackBody<Args...>;

    std::vector<std::shared_ptr<CallbackBodyType>> callbackBodies;

    Connection connect(typename CallbackBodyType::FunctionSignature func);

    void
    disconnect(uint64_t index)
    {
        for (auto body : this->callbackBodies) {
            if (body->index == index) {
                body->disconnect();
            }
        }

        for (typename std::vector<Callback>::size_type i = 0, n = this->callbacks.size(); i < n;
             ++i) {
            if (this->callbacks[i].index == index) {
                this->callbacks.erase(std::begin(this->callbacks) + i);
                return true;
            }
        }

        return false;
    }

    void
    disconnectAll()
    {
        for (auto body : this->callbackBodies) {
            body->disconnect();
        }
    }

private:
    std::atomic<uint64_t> latestConnection;

    uint64_t
    nextConnection()
    {
        return ++this->latestConnection;
    }
};

/// Bolt Signals (1-time use)
// connect is fast
// disconnect doesn't exist
// invoke is fast
template <class... Args>
class BaseBoltSignal
{
protected:
    BaseBoltSignal() = default;

protected:
    typedef std::function<void(Args...)> CallbackType;

public:
    void
    connect(CallbackType cb)
    {
        this->callbacks.push_back(std::move(cb));
    }

protected:
    std::vector<CallbackType> callbacks;
};

}  // namespace detail

// Signal that takes 1+ arguments
template <typename... Args>
class Signal : public detail::BaseSignal<Args...>
{
public:
    void
    invokeOne(uint64_t index, Args... args)
    {
        // This might not be fully functional atm
        for (auto &callback : this->callbackBodies) {
            if (callback->index == index) {
                if (!callback->blocked) {
                    callback.func(args...);
                }

                break;
            }
        }
    }

    void
    invoke(Args... args)
    {
        for (auto it = this->callbackBodies.begin(); it != this->callbackBodies.end();) {
            std::shared_ptr<CallbackBodyType> &callback = *it;

            if (!callback->isConnected()) {
                // Clean up disconnected callbacks
                it = this->callbackBodies.erase(it);
                continue;
            }

            if (!callback->isBlocked()) {
                callback->func(args...);
            }

            ++it;
        }
    }
};

// Signal that takes no arguments
class NoArgSignal : public detail::BaseSignal<>
{
public:
    void
    invokeOne(uint64_t index)
    {
        // This might not be fully functional atm
        for (auto &callback : this->callbackBodies) {
            if (!callback->isConnected()) {
                continue;
            }

            if (callback->index == index) {
                if (!callback->isBlocked()) {
                    callback->func();
                }

                break;
            }
        }
    }

    void
    invoke()
    {
        for (auto it = this->callbackBodies.begin(); it != this->callbackBodies.end();) {
            auto &callback = *it;

            if (!callback->isConnected()) {
                // Clean up disconnected callbacks
                it = this->callbackBodies.erase(it);
                continue;
            }

            if (!callback->isBlocked()) {
                callback->func();
            }

            ++it;
        }
    }
};

class NoArgBoltSignal : public detail::BaseBoltSignal<>
{
public:
    void
    invoke()
    {
        for (auto &callback : this->callbacks) {
            callback();
        }

        this->callbacks.clear();
    }
};

template <class... Args>
class BoltSignal : public detail::BaseBoltSignal<Args...>
{
public:
    void
    invoke(Args... args)
    {
        for (auto &callback : this->callbacks) {
            callback.func(args...);
        }

        this->callbacks.clear();
    }
};

namespace detail {

template <typename... Args>
Connection
BaseSignal<Args...>::connect(typename CallbackBodyType::FunctionSignature func)
{
    uint64_t connectionIndex = this->nextConnection();

    std::shared_ptr<CallbackBodyType> cb(new CallbackBodyType(connectionIndex));
    cb->func = std::move(func);

    std::weak_ptr<CallbackBodyType> weakCallback(cb);

    this->callbackBodies.emplace_back(std::move(cb));

    return Connection(weakCallback);
}

}  // namespace detail

}  // namespace Signals
}  // namespace pajlada
