//
// Created by AmazingBuff on 2025/6/12.
//

#ifndef DYNAMIC_VECTOR_H
#define DYNAMIC_VECTOR_H

#include "astd/base/define.h"
#include "astd/base/logger.h"
#include "astd/base/util.h"
#include "astd/trait/trait.h"
#include "astd/memory/allocator.h"

AMAZING_NAMESPACE_BEGIN

template <typename Tp, template <typename> typename Alloc = Allocator>
class DynamicVector
{
    using allocator = Alloc<Tp>;
public:
    static constexpr size_t Block_Size = 1 << 9;
    static constexpr size_t Block_Index_Mask = Block_Size - 1;
public:
    class Iterator
    {
    public:
        Iterator(Tp** ref_data, size_t index) : m_ref_data(ref_data), m_index(index) {}

        Iterator& operator++()
        {
            ++m_index;
            return *this;
        }

        Iterator& operator--()
        {
            --m_index;
            return *this;
        }

        Tp& operator*()
        {
            size_t bucket = division_up(m_index, Block_Size);
            size_t index = m_index & Block_Index_Mask;
            return m_ref_data[bucket][index];
        }

        const Tp& operator*() const
        {
            size_t bucket = division_up(m_index, Block_Size);
            size_t index = m_index & Block_Index_Mask;
            return m_ref_data[bucket][index];
        }

        Tp* operator->()
        {
            size_t bucket = division_up(m_index, Block_Size);
            size_t index = m_index & Block_Index_Mask;
            return &m_ref_data[bucket][index];
        }

        NODISCARD bool operator==(const Iterator& other) const
        {
            return m_index == other.m_index;
        }

        NODISCARD bool operator!=(const Iterator& other) const
        {
            return m_index != other.m_index;
        }

    private:
        Tp** m_ref_data;
        size_t m_index;
    };
public:
    DynamicVector() : m_data(nullptr), m_bucket(0), m_size(0), m_capacity(0) {}
    ~DynamicVector()
    {
        for (size_t i = 0; i < m_bucket; ++i)
            allocator::deallocate(m_data[i]);
        Alloc<Tp*>::deallocate(m_data);
    }

    explicit DynamicVector(size_t size) : m_size(size)
    {
        m_bucket = division_up(size, Block_Size);
        m_capacity = m_bucket * Block_Size;

        m_data = Alloc<Tp*>::allocate(m_bucket);
        for (size_t i = 0; i < m_bucket; ++i)
            m_data[i] = allocator::allocate(Block_Size);
    }

    void resize(size_t size)
    {
        if (size > m_capacity)
            reserve(size);
        m_size = size;
    }

    void reserve(size_t capacity)
    {
        size_t bucket = division_up(capacity, Block_Size);
        if (bucket == m_bucket)
            return;

        Tp** new_data = Alloc<Tp*>::reallocate(m_data, bucket);
        if (new_data != m_data)
        {
            std::memcpy(new_data, m_data, sizeof(Tp*) * std::min(bucket, m_bucket));
            for (size_t i = bucket; i < m_bucket; ++i)
                allocator::deallocate(m_data[i]);
            Alloc<Tp*>::deallocate(m_data);
            m_data = new_data;
        }
        else
        {
            for (size_t i = bucket; i < m_bucket; ++i)
                allocator::deallocate(m_data[i]);
        }
        m_bucket = bucket;
        m_capacity = bucket * Block_Size;
    }

    NODISCARD size_t size() const
    {
        return m_size;
    }

    NODISCARD size_t capacity() const
    {
        return m_capacity;
    }

    NODISCARD bool empty() const
    {
        return m_size == 0;
    }

    NODISCARD Tp& operator[](size_t index)
    {
        return m_data[index / Block_Size][index % Block_Size];
    }

    NODISCARD const Tp& operator[](size_t index) const
    {
        return m_data[index / Block_Size][index % Block_Size];
    }

    Iterator begin()
    {
        return Iterator(m_data, 0);
    }

    Iterator const begin() const
    {
        return Iterator(m_data, 0);
    }

    Iterator end()
    {
        return Iterator(m_data, m_size);
    }

    Iterator const end() const
    {
        return Iterator(m_data, m_size);
    }

private:
    Tp** m_data;
    size_t m_bucket;
    size_t m_size;
    size_t m_capacity;
};


AMAZING_NAMESPACE_END

#endif //DYNAMIC_VECTOR_H
