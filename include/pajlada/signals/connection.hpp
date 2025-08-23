#pragma once

#include <cassert>
#include <cstdint>
#include <functional>
#include <memory>

namespace pajlada {
namespace Signals {

namespace detail {

class CallbackBodyBase
{
protected:
    explicit CallbackBodyBase() = default;

public:
    virtual ~CallbackBodyBase() = default;

    void
    addRef()
    {
        ++this->subscriberRefCount;
    }

    bool
    disconnect()
    {
        assert(this->subscriberRefCount > 0);

        --this->subscriberRefCount;

        return true;
    }

    bool
    isConnected() const
    {
        return this->subscriberRefCount > 0;
    }

    [[nodiscard]] unsigned
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
    bool blocked{false};

    // probably need to actually mutex-lock anything that would change our connected state
    uint32_t subscriberRefCount{0};
};

template <typename... Args>
class CallbackBody : public CallbackBodyBase
{
public:
    explicit CallbackBody() = default;

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
        : weakCallbackBody(std::move(other.weakCallbackBody))
    {
    }

    Connection &
    operator=(Connection &&other) noexcept
    {
        if (&other == this) {
            return *this;
        }

        this->disconnect();
        this->weakCallbackBody = std::move(other.weakCallbackBody);
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

    [[nodiscard]] bool
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

    [[nodiscard]] SubscriberRefCountResponse
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

}  // namespace Signals
}  // namespace pajlada
