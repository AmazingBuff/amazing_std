//
// Created by AmazingBuff on 25-6-1.
//

#ifndef FUNCTIONAL_H
#define FUNCTIONAL_H

#include "astd/memory/allocator.h"
#include "trait.h"

AMAZING_NAMESPACE_BEGIN

INTERNAL_NAMESPACE_BEGIN

static constexpr size_t Small_Function_Size = 64;

template <typename R, typename... Args>
class IFunctional
{
public:
    virtual ~IFunctional() = default;
    virtual R call(Args&&... args) = 0;
    virtual R call(Args&&... args) const = 0;
    virtual IFunctional* clone(uint8_t* address) const = 0;
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
    R call(Args&&... args) const override
    {
        return m_value(std::forward<Args>(args)...);
    }
    IFunctional<R, Args...>* clone(uint8_t* address) const override
    {
        if constexpr (sizeof(FunctionalImpl) <= Small_Function_Size)
            return new (address) FunctionalImpl(m_value);
        else
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
    Functional() : m_functional(nullptr), m_stack{} {}

    template <typename F>
    Functional(F f) : m_stack{}
    {
        using FunctionImplType = Internal::FunctionalImpl<F, R, Args...>;
        if constexpr (sizeof(FunctionImplType) <= Internal::Small_Function_Size)
            m_functional = new (m_stack) FunctionImplType(std::move(f));
        else
            m_functional = PLACEMENT_NEW(FunctionImplType, sizeof(FunctionImplType), std::move(f));
    }

    Functional(const Functional& other) : m_functional(nullptr), m_stack{}
    {
        if (other.m_functional)
            m_functional = other.m_functional->clone(m_stack);
    }

    ~Functional()
    {
        if (reinterpret_cast<uintptr_t>(m_functional) != reinterpret_cast<uintptr_t>(m_stack))
            PLACEMENT_DELETE(FunctionType, m_functional);
        m_functional = nullptr;
    }

    template <typename F>
    Functional& operator=(F f)
    {
        if (m_functional != nullptr && reinterpret_cast<uintptr_t>(m_functional) != reinterpret_cast<uintptr_t>(m_stack))
            PLACEMENT_DELETE(FunctionType, m_functional);

        using FunctionImplType = Internal::FunctionalImpl<F, R, Args...>;
        if constexpr (sizeof(FunctionImplType) <= Internal::Small_Function_Size)
            m_functional = new (m_stack) FunctionImplType(std::move(f));
        else
            m_functional = PLACEMENT_NEW(FunctionImplType, sizeof(FunctionImplType), std::move(f));

        return *this;
    }

    Functional& operator=(const Functional& other)
    {
        if (this != &other)
        {
            if (m_functional != nullptr && reinterpret_cast<uintptr_t>(m_functional) != reinterpret_cast<uintptr_t>(m_stack))
                PLACEMENT_DELETE(FunctionType, m_functional);

            if (other.m_functional)
                m_functional = other.m_functional->clone(m_stack);
            else
            {
                m_functional = nullptr;
                std::memcpy(m_stack, other.m_stack, sizeof(Internal::Small_Function_Size));
            }
        }

        return *this;
    }

    R operator()(Args&&... args)
    {
        return m_functional->call(std::forward<Args>(args)...);
    }

    R operator()(Args&&... args) const
    {
        return m_functional->call(std::forward<Args>(args)...);
    }

    explicit operator bool() const
    {
        return m_functional != nullptr;
    }

private:
    FunctionType* m_functional;
    uint8_t m_stack[Internal::Small_Function_Size]; // stack space for small function object, avoid heap allocation
};


AMAZING_NAMESPACE_END


#endif //FUNCTIONAL_H
