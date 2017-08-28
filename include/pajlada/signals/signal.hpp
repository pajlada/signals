#pragma once

#include <atomic>
#include <functional>
#include <vector>

namespace pajlada {
namespace Signals {

struct Connection;

namespace detail {

class UltraBaseSignal
{
public:
    virtual bool disconnect(uint64_t index) = 0;
    virtual bool block(uint64_t index) = 0;
    virtual bool unblock(uint64_t index) = 0;
};

template <class... Args>
class BaseSignal : public UltraBaseSignal
{
protected:
    BaseSignal()
        : latestConnection(0)
    {
    }

    using CallbackType = std::function<void(Args...)>;

    struct Callback {
        uint64_t index;
        bool blocked = false;
        CallbackType func;
    };

public:
    // Return connection ID
    // CallbackType = lambda/std::bind'ed function etc
    // TODO: see if we can use && etc on the argument here, idk c++ tho
    Connection connect(CallbackType func);

    // NOTE: I'm not sure if this is threadsafe together with invoke
    virtual bool
    disconnect(uint64_t index) override
    {
        for (typename std::vector<Callback>::size_type i = 0, n = this->callbacks.size(); i < n;
             ++i) {
            if (this->callbacks[i].index == index) {
                this->callbacks.erase(std::begin(this->callbacks) + i);
                return true;
            }
        }

        return false;
    }

    virtual bool
    block(uint64_t index) override
    {
        for (auto &cb : this->callbacks) {
            if (cb.index == index) {
                cb.blocked = true;
                return true;
            }
        }

        return false;
    }

    virtual bool
    unblock(uint64_t index) override
    {
        for (auto &cb : this->callbacks) {
            if (cb.index == index) {
                cb.blocked = false;
                return true;
            }
        }

        return false;
    }

protected:
    std::vector<Callback> callbacks;

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

struct Connection {
    Connection()
        : signal(nullptr)
        , index(0)
        , blockCounter(0)
    {
    }

    // Connection(const Connection &other) = delete;

    Connection(detail::UltraBaseSignal *_signal, uint64_t _index)
        : signal(_signal)
        , index(_index)
    {
    }

    /*
    Connection(Connection &&other)
        : signal(std::move(other.signal))
        , index(std::move(other.index))
        , blockCounter(std::move(other.blockCounter))
    {
    }

    // Connection &operator=(const Connection &other) = delete;

    Connection &
    operator=(Connection &&other)
    {
        this->signal = std::move(other.signal);
        this->index = std::move(other.index);
        this->blockCounter = std::move(other.blockCounter);

        return *this;
    }
    */

    detail::UltraBaseSignal *signal;
    uint64_t index;

    bool
    disconnect()
    {
        if (this->signal) {
            return this->signal->disconnect(this->index);
        }

        return false;
    }

    bool
    block()
    {
        if (this->signal) {
            ++this->blockCounter;
            if (this->blockCounter == 1) {
                return this->signal->block(this->index);
            }

            return false;
        }

        return false;
    }

    bool
    unblock()
    {
        if (this->signal) {
            if (this->blockCounter == 0) {
                // Cannot unblock more times than you blocked forsenE
                return false;
            }

            --this->blockCounter;
            if (this->blockCounter == 0) {
                return this->signal->unblock(this->index);
            }

            return false;
        }

        return false;
    }

private:
    uint32_t blockCounter = 0;
};

struct ScopedConnection {
    ScopedConnection(Connection &&_connection)
        : connection(std::move(_connection))
    {
    }

    ~ScopedConnection()
    {
        this->connection.disconnect();
    }

    Connection connection;
};

struct ScopedBlock {
    ScopedBlock() = delete;

    ScopedBlock(Connection &_connection)
        : connection(_connection)
    {
        this->connection.block();
    }

    ~ScopedBlock()
    {
        this->connection.unblock();
    }

    Connection &connection;
};

// Signal that takes 1+ arguments
template <class... Args>
class Signal : public detail::BaseSignal<Args...>
{
public:
    void
    invoke(Args... args)
    {
        for (auto &callback : this->callbacks) {
            if (!callback.blocked) {
                callback.func(args...);
            }
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

namespace detail {

template <class... Args>
Connection
BaseSignal<Args...>::connect(CallbackType func)
{
    uint64_t connectionIndex = this->nextConnection();

    Callback cb;
    cb.func = std::move(func);
    cb.index = connectionIndex;

    this->callbacks.emplace_back(std::move(cb));

    return Connection(this, connectionIndex);
}

}  // namespace detail

}  // namespace Signals
}  // namespace pajlada
