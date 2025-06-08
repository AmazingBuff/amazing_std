//
// Created by AmazingBuff on 25-6-1.
//

#ifndef FUNCTIONAL_H
#define FUNCTIONAL_H

#include "trait.h"
#include "memory/allocator.h"

AMAZING_NAMESPACE_BEGIN

INTERNAL_NAMESPACE_BEGIN

template <typename R, typename... Args>
class IFunctional
{
public:
    virtual ~IFunctional() = default;
    virtual R call(Args&&... args) = 0;
    virtual IFunctional* clone() const = 0;
};

template <typename T, typename R, typename... Args>
class FunctionalImpl final : public IFunctional<R, Args...>
{
public:
    explicit FunctionalImpl(T value) : m_value(std::move(value)) {}
    R call(Args&&... args) override
    {
        return m_value(std::forward<Args>(args)...);
    }
    IFunctional<R, Args...>* clone() const override
    {
        return PLACEMENT_NEW(FunctionalImpl, sizeof(FunctionalImpl), std::move(m_value));
    }
private:
    T m_value;
};


INTERNAL_NAMESPACE_END

// a function wrapper, support function pointer and lambda expression
template <typename F>
class Functional;

// only for basic function type, wrap with lambda expression when using member function pointer
template <typename R, typename... Args>
class Functional<R(Args...)>
{
    using FunctionType = Internal::IFunctional<R, Args...>;
public:
    Functional() : m_functional(nullptr) {}

    template <typename F>
    Functional(F f)
    {
        using FunctionImplType = Internal::FunctionalImpl<F, R, Args...>;
        m_functional = PLACEMENT_NEW(FunctionImplType, sizeof(FunctionImplType), std::move(f));
    }

    ~Functional()
    {
        PLACEMENT_DELETE(FunctionType, m_functional);
    }

    template <typename F>
    Functional& operator=(F f)
    {
        if (m_functional != nullptr)
            PLACEMENT_DELETE(FunctionType, m_functional);

        m_functional = PLACEMENT_NEW((Internal::FunctionalImpl<F, R, Args...>), sizeof(Internal::FunctionalImpl<F, R, Args...>), std::move(f));

        return *this;
    }

    Functional& operator=(const Functional& other)
    {
        if (this != &other)
        {
            if (m_functional != nullptr)
                PLACEMENT_DELETE(FunctionType, m_functional);

            m_functional = other.m_functional->clone();
        }

        return *this;
    }

    R operator()(Args&&... args)
    {
        return m_functional->call(std::forward<Args>(args)...);
    }

private:
    FunctionType* m_functional;
};


AMAZING_NAMESPACE_END


#endif //FUNCTIONAL_H
