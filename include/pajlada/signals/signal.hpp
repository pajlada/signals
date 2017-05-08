#pragma once

#include <atomic>
#include <functional>
#include <vector>

namespace pajlada {
namespace Signals {

typedef uint64_t ConnectionType;

namespace detail {

template <class... Args>
class BaseSignal
{
protected:
    BaseSignal()
        : latestConnection(0)
    {
    }

public:
    using Connection = ConnectionType;

protected:
    using CallbackType = std::function<void(Args...)>;

    struct Callback {
        Connection connection;
        CallbackType func;
    };

public:
    // Return connection ID
    // CallbackType = lambda/std::bind'ed function etc
    // TODO: see if we can use && etc on the argument here, idk c++ tho
    Connection
    connect(CallbackType cb)
    {
        Connection connection(this->nextConnection());

        this->callbacks.push_back(Callback{connection, std::move(cb)});

        return connection;
    }

    // NOTE: I'm not sure if this is threadsafe together with invoke
    bool
    disconnect(Connection connection)
    {
        for (typename std::vector<Callback>::size_type
                 i = 0,
                 n = this->callbacks.size();
             i < n; ++i) {
            if (this->callbacks[i].connection == connection) {
                this->callbacks.erase(std::begin(this->callbacks) + i);
                return true;
            }
        }

        return false;
    }

protected:
    std::vector<Callback> callbacks;

private:
    std::atomic<Connection> latestConnection;

    Connection
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
template <class... Args>
class Signal : public detail::BaseSignal<Args...>
{
public:
    void
    invoke(Args... args)
    {
        for (auto &callback : this->callbacks) {
            callback.func(args...);
        }
    }
};

// Signal that takes no arguments
class NoArgSignal : public detail::BaseSignal<>
{
public:
    void
    invoke()
    {
        for (auto &callback : this->callbacks) {
            callback.func();
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

}  // namespace Signals
}  // namespace pajlada
