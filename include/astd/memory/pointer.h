//
// Created by AmazingBuff on 2025/4/3.
//

#pragma once

#include "allocator.h"

AMAZING_NAMESPACE_BEGIN

INTERNAL_NAMESPACE_BEGIN

struct RefCounter
{
    size_t use_count;
    size_t weak_count;
};

template<typename Tp, template <typename> typename Alloc>
class Ptr
{
    using allocator_counter = Alloc<RefCounter>;
public:
    Ptr() : m_ptr(nullptr)
    {
        m_ref = allocator_counter::allocate(1);
        m_ref->use_count = 1;
        m_ref->weak_count = 1;
    }

    virtual ~Ptr() = 0;

    explicit operator bool() const noexcept
    {
        return m_ptr != nullptr;
    }

protected:
    Tp* m_ptr;
    RefCounter* m_ref;
};

INTERNAL_NAMESPACE_END

template<typename Tp, template <typename> typename Alloc = Allocator>
class SharedPtr final : public Internal::Ptr<Tp, Alloc>
{
    using Ptr = Internal::Ptr<Tp, Alloc>;
    using allocator = Alloc<Tp>;
    using allocator_counter = Alloc<Internal::RefCounter>;
public:
    SharedPtr() = default;

    ~SharedPtr() override
    {
        --Ptr::m_ref->use_count;
        --Ptr::m_ref->weak_count;
        if (Ptr::m_ref->use_count == 0)
        {
            allocator::deallocate(Ptr::m_ptr);
            if (Ptr::m_ref->weak_count == 0)
                allocator_counter::deallocate(Ptr::m_ref);
        }
    }

    template <typename... Args>
    requires(std::is_constructible_v<Tp, Args...>)
    explicit SharedPtr(Args&&... args)
    {
        Tp* p = allocator::allocate(1);
        Ptr::m_ptr = new (p) Tp(std::forward<Args>(args)...);
    }

    SharedPtr(const SharedPtr& rhs)
    {
        Ptr::m_ptr = rhs.m_ptr;
        Ptr::m_ref = rhs.m_ref;
        ++Ptr::m_ref->use_count;
    }

    SharedPtr& operator=(const SharedPtr& rhs)
    {
        Ptr::m_ptr = rhs.m_ptr;
        Ptr::m_ref = rhs.m_ref;
        ++Ptr::m_ref->use_count;
        return *this;
    }

    NODISCARD Tp* get() const
    {
        return Ptr::m_ptr;
    }

    NODISCARD size_t use_count() const
    {
        return Ptr::m_ref->use_count;
    }

    NODISCARD Tp* operator->() const
    {
        return Ptr::m_ptr;
    }
};


template<typename Tp, template <typename> typename Alloc = Allocator>
class WeakPtr : public Internal::Ptr<Tp, Alloc>
{
    using Ptr = Internal::Ptr<Tp, Alloc>;
    using allocator = Alloc<Tp>;
public:
    WeakPtr() = default;

    ~WeakPtr() override
    {
        --Ptr::m_ref->weak_count;
    }

    explicit WeakPtr(const SharedPtr<Tp>& rhs)
    {
        Ptr::m_ptr = rhs.m_ptr;
        Ptr::m_ref = rhs.m_ref;
        ++Ptr::m_ref->weak_count;
    }

    WeakPtr& operator=(const SharedPtr<Tp>& rhs)
    {
        Ptr::m_ptr = rhs.m_ptr;
        Ptr::m_ref = rhs.m_ref;
        ++Ptr::m_ref->weak_count;
        return *this;
    }

    WeakPtr& operator=(const WeakPtr<Tp>& rhs)
    {
        Ptr::m_ptr = rhs.m_ptr;
        Ptr::m_ref = rhs.m_ref;
        ++Ptr::m_ref->weak_count;
        return *this;
    }
};


template<typename Tp, template <typename> typename Alloc = Allocator>
class UniquePtr
{
    using allocator = Alloc<Tp>;
public:
    explicit UniquePtr(Tp* ptr = nullptr) : m_ptr(ptr) {}
    ~UniquePtr()
    {
        allocator::deallocate(m_ptr);
        m_ptr = nullptr;
    }

    template <typename... Args>
    requires(std::is_constructible_v<Tp, Args...>)
    explicit UniquePtr(Args&&... args)
    {
        Tp* p = allocator::allocate(1);
        m_ptr = new (p) Tp(std::forward<Args>(args)...);
    }

    UniquePtr(const UniquePtr&) = delete;
    UniquePtr& operator=(const UniquePtr&) = delete;

    NODISCARD Tp* get() const
    {
        return m_ptr;
    }

    NODISCARD Tp* operator->() const
    {
        return m_ptr;
    }

private:
    Tp* m_ptr;
};


AMAZING_NAMESPACE_END
