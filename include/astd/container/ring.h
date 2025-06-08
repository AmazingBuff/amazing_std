//
// Created by AmazingBuffer on 2025/4/10.
//

#pragma once

#include "astd/base/util.h"
#include "astd/trait/trait.h"
#include "astd/memory/allocator.h"
#include <atomic>

AMAZING_NAMESPACE_BEGIN

INTERNAL_NAMESPACE_BEGIN

static constexpr size_t k_Ring_Buffer_Size = 32;

INTERNAL_NAMESPACE_END

template<typename Tp, template <typename> typename Alloc = Allocator>
class Ring
{
    using allocator = Alloc<Tp>;
public:
    explicit Ring(size_t size = Internal::k_Ring_Buffer_Size) : m_read(0), m_write(0)
    {
        m_size = align_to(size, 2ull);
        m_data = allocator::allocate(m_size);
    }

    size_t write(const Tp* buffer, size_t size)
    {
        size_t read = m_read.load(std::memory_order_acquire);
        size_t write = m_write;

        size_t len = std::min(size, m_size + read - write);
        size_t remaining = std::min(len, m_size - (write & (m_size - 1)));
        memcpy(m_data + (write & (m_size - 1)), buffer, remaining);
        memcpy(m_data, buffer + remaining, len - remaining);

        m_write.store(write + len, std::memory_order_release);

        return len;
    }

    size_t read(Tp* buffer, size_t size)
    {
        size_t write = m_write.load(std::memory_order_acquire);
        size_t read = m_read;

        size_t len = std::min(size, write - read);
        size_t remaining = std::min(len, m_size - (read & (m_size - 1)));
        memcpy(buffer, m_data + (read & (m_size - 1)), remaining);
        memcpy(buffer + remaining, m_data, len - remaining);

        m_read.store(read + len, std::memory_order_release);
        return len;
    }

    NODISCARD size_t size() const
    {
        return m_write - m_read;
    }

private:
    Tp* m_data;
    size_t                  m_size;
    std::atomic<size_t>     m_read;
    std::atomic<size_t>     m_write;
};


AMAZING_NAMESPACE_END
