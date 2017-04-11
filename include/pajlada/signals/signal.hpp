#pragma once

#include <atomic>
#include <functional>
#include <vector>

namespace pajlada {
namespace signals {

namespace detail {

template <class... Args>
class BaseSignal
{
protected:
    BaseSignal() = default;

public:
    using Connection = uint64_t;

protected:
    typedef std::function<void(Args...)> CallbackType;

    struct Callback {
        Connection connection;
        CallbackType func;
    };

public:
    // Return connection ID (we'll use uint64_t)
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
        for (std::vector<Callback>::size_type i = 0, n = this->callbacks.size();
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
    std::atomic<Connection> latestConnection = 0;

    Connection
    nextConnection()
    {
        return ++this->latestConnection;
    }
};

}  // anonymous namespace

// Signal that takes 1+ arguments
template <class... Args>
class Signal : public detail::BaseSignal<Args...>
{
public:
    void
    invoke(Args... args)
    {
        for (Callback &callback : this->callbacks) {
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
        for (Callback &callback : this->callbacks) {
            callback.func();
        }
    }
};

}  // namespace signals
}  // namespace pajlada
