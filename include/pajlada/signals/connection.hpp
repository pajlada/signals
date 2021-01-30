#pragma once

#include <atomic>
#include <functional>
#include <memory>

namespace pajlada {
namespace Signals {

namespace detail {

class CallbackBodyBase
{
protected:
    CallbackBodyBase() = delete;

    explicit CallbackBodyBase(uint64_t _index)
        : index(_index)
        , connected(true)
        , blocked(false)
        , subscriberRefCount(0)
    {
    }

public:
    const uint64_t index;

    virtual ~CallbackBodyBase() = default;

    void
    addRef()
    {
        ++this->subscriberRefCount;
    }

    bool
    disconnect()
    {
        --this->subscriberRefCount;

        if (this->subscriberRefCount > 0) {
            return false;
        }

        if (this->connected) {
            this->connected = false;

            return true;
        }

        return false;
    }

    bool
    isConnected() const
    {
        return this->connected;
    }

    unsigned
    getSubscriberRefCount() const
    {
        return this->subscriberRefCount;
    }

    bool
    block()
    {
        if (this->blocked) {
            return false;
        }

        this->blocked = true;

        return true;
    }

    bool
    unblock()
    {
        if (!this->blocked) {
            return false;
        }

        this->blocked = false;

        return true;
    }

    bool
    isBlocked() const
    {
        return this->blocked;
    }

private:
    // probably need to actually mutex-lock anything that would change our connected state
    std::atomic<bool> connected;
    bool blocked;

    unsigned subscriberRefCount;
};

template <typename... Args>
class CallbackBody : public CallbackBodyBase
{
public:
    CallbackBody() = delete;

    explicit CallbackBody(uint64_t _index)
        : CallbackBodyBase(_index)
    {
    }

    virtual ~CallbackBody() = default;

    using FunctionSignature = std::function<void(Args...)>;

    FunctionSignature func;

    void
    invoke(Args... args)
    {
        if (!this->isConnected()) {
            return;
        }

        this->func(std::forward<Args>(args)...);
    }
};

}  // namespace detail

class Connection
{
public:
    Connection() = default;

    Connection(const Connection &other)
    {
        this->connect(other.weakCallbackBody);
    }

    Connection(const std::weak_ptr<detail::CallbackBodyBase> &connectionBody)
    {
        this->connect(connectionBody);
    }

    Connection(Connection &&other) noexcept
    {
        this->connect(other.weakCallbackBody);
        other.disconnect();
    }

    Connection &
    operator=(Connection &&other) noexcept
    {
        if (&other == this) {
            return *this;
        }

        this->connect(other.weakCallbackBody);
        other.disconnect();

        return *this;
    }

    Connection &
    operator=(const Connection &other)
    {
        if (&other == this) {
            return *this;
        }

        // Connect to other's body
        this->connect(other.weakCallbackBody);

        return *this;
    }

    void
    connect(std::weak_ptr<detail::CallbackBodyBase> weakBody)
    {
        // Disconnect from a previous body
        this->disconnect();

        auto body = weakBody.lock();

        if (body) {
            this->weakCallbackBody = body;

            body->addRef();
        }
    }

    bool
    disconnect()
    {
        auto connectionBody(this->weakCallbackBody.lock());
        if (!connectionBody) {
            return false;
        }

        connectionBody->disconnect();

        this->weakCallbackBody.reset();

        return true;
    }

    bool
    isConnected()
    {
        auto connectionBody(this->weakCallbackBody.lock());
        if (!connectionBody) {
            return false;
        }

        return connectionBody->isConnected();
    }

    struct SubscriberRefCountResponse {
        unsigned count;
        bool connected;
    };

    SubscriberRefCountResponse
    getSubscriberRefCount()
    {
        auto connectionBody(this->weakCallbackBody.lock());
        if (!connectionBody) {
            return {0, false};
        }

        return {connectionBody->getSubscriberRefCount(), true};
    }

    bool
    block()
    {
        auto connectionBody(this->weakCallbackBody.lock());
        if (!connectionBody) {
            return false;
        }

        return connectionBody->block();
    }

    bool
    unblock()
    {
        auto connectionBody(this->weakCallbackBody.lock());
        if (!connectionBody) {
            return false;
        }

        return connectionBody->unblock();
    }

    bool
    isBlocked()
    {
        auto connectionBody(this->weakCallbackBody.lock());
        if (!connectionBody) {
            return false;
        }

        return connectionBody->isBlocked();
    }

    template <typename... Args>
    void
    invoke(Args... args)
    {
        auto connectionBody(this->weakCallbackBody.lock());
        if (!connectionBody) {
            return;
        }

        try {
            auto advancedConnectionBody =
                std::static_pointer_cast<detail::CallbackBody<Args...>>(
                    connectionBody);
            advancedConnectionBody->invoke(std::forward<Args>(args)...);
        } catch (...) {
            // TODO: Figure out which exceptino is thrown here
        }
    }

private:
    std::weak_ptr<detail::CallbackBodyBase> weakCallbackBody;
};

class ScopedConnection
{
    Connection connection;

public:
    ScopedConnection() = default;

    ScopedConnection(ScopedConnection &&other) noexcept
        : connection(std::move(other.connection))
    {
    }

    ScopedConnection(Connection &&_connection) noexcept
        : connection(std::move(_connection))
    {
    }

    // Copying a connection may have dangerous unseen side-effects, therefore they may not happen unless explicitly converted
    explicit ScopedConnection(const Connection &other)
        : connection(other)
    {
    }

    // Copying a connection may have dangerous unseen side-effects, therefore they may not happen unless explicitly converted
    explicit ScopedConnection(const ScopedConnection &other)
        : connection(other.connection)
    {
    }

    ScopedConnection &
    operator=(const Connection &other)
    {
        this->connection = other;

        return *this;
    }

    ScopedConnection &
    operator=(const ScopedConnection &other)
    {
        this->connection = other.connection;

        return *this;
    }

    ScopedConnection &
    operator=(ScopedConnection &&other) noexcept
    {
        if (&other == this) {
            return *this;
        }

        this->connection = std::move(other.connection);

        return *this;
    }

    ScopedConnection &
    operator=(Connection &&other) noexcept
    {
        this->connection = std::move(other);

        return *this;
    }

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
