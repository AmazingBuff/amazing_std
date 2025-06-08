//
// Created by AmazingBuff on 2025/4/29.
//

#ifndef HASH_H
#define HASH_H

#include "astd/container/vector.h"

AMAZING_NAMESPACE_BEGIN

INTERNAL_NAMESPACE_BEGIN

static constexpr size_t Bucket_Count = 8;

class Probe
{
public:
    Probe() : m_counter(0) {}

    void increment() { m_counter++; }
    void decrement()
    {
        if (m_counter > 0)
            m_counter--;
    }

    size_t operator()(size_t hash, size_t bucket_count) const
    {
        return probe(hash, m_counter, bucket_count);
    }
private:
    static size_t probe(size_t hash, size_t index, size_t bucket_count)
    {
        size_t p = (hash >> 7) % bucket_count;
        for (size_t i = 1; i < index; i++)
            p = (p + i) % bucket_count;

        return p;
    }
private:
    size_t m_counter;
};


enum class ElementFlag : uint8_t
{
    e_invalid,
    e_valid,
    e_end
};

template <typename Tp>
struct HashNode
{
    ElementFlag flag;
    Tp val;
};


template <typename Trait>
class Hash
{
    using key_type = typename Trait::key_type;
    using value_type = typename Trait::value_type;
    using key_hash = typename Trait::key_hash;
    using key_equal = typename Trait::key_equal;
    using value_hash = typename Trait::value_hash;
    using node_type = typename Trait::node_type;
    using allocator = typename Trait::allocator;

    static constexpr size_t max_bucket_size = Trait::max_bucket_size;
    static constexpr size_t max_load_factor_numerator = Trait::max_load_factor_numerator;
    static constexpr size_t max_load_factor_denominator = Trait::max_load_factor_denominator;
public:
    class Iterator
    {
    public:
        Iterator() : m_node(nullptr) {}
        explicit Iterator(node_type* node) : m_node(node) {}

        Iterator& operator++()
        {
            do
            {
                ++m_node;
            } while (m_node->flag == ElementFlag::e_invalid);
            return *this;
        }

        Iterator& operator--()
        {
            do
            {
                --m_node;
            } while (m_node->flag == ElementFlag::e_invalid);
            return *this;
        }

        value_type& operator*()
        {
            return m_node->val;
        }

        const value_type& operator*() const
        {
            return m_node->val;
        }

        value_type* operator->()
        {
            return &m_node->val;
        }

        NODISCARD bool operator==(const Iterator& other) const
        {
            return m_node == other.m_node;
        }

        NODISCARD bool operator!=(const Iterator& other) const
        {
            return m_node != other.m_node;
        }

    private:
        node_type* m_node;

        friend class Hash;
    };
public:
    Hash() : m_size(0)
    {
        m_bucket_count = Bucket_Count;
        size_t element_size = m_bucket_count * max_bucket_size + 1;
        m_buckets = allocator::allocate(element_size * sizeof(node_type));
        for (size_t i = 0; i < element_size; i++)
            new (m_buckets + i) node_type();

        // end flag
        m_buckets[element_size - 1].flag = ElementFlag::e_end;
    }

    Hash(const Hash& other) : m_bucket_count(other.m_bucket_count), m_size(other.m_size), m_probe(other.m_probe)
    {
        size_t element_size = m_bucket_count * max_bucket_size + 1;
        m_buckets = allocator::allocate(element_size * sizeof(node_type));
        for (size_t i = 0; i < element_size; i++)
            new (m_buckets + i) node_type();

        for (size_t i = 0; i < element_size; i++)
            m_buckets[i] = other.m_buckets[i];

        // end flag
        m_buckets[element_size - 1].flag = ElementFlag::e_end;
    }

    Hash(Hash&& other) noexcept : m_bucket_count(other.m_bucket_count), m_size(other.m_size)
    {
        swap(other);
    }

    ~Hash()
    {
        for (size_t i = 0; i < m_bucket_count * max_bucket_size + 1; i++)
            m_buckets[i].~node_type();
        deallocate(m_buckets);
        m_bucket_count = 0;
        m_size = 0;
    }

    Hash& operator=(const Hash& other)
    {
        if (this != &other)
        {
            for (size_t i = 0; i < m_bucket_count * max_bucket_size + 1; i++)
                m_buckets[i].~node_type();
            deallocate(m_buckets);

            m_bucket_count = other.m_bucket_count;
            m_size = other.m_size;
            m_probe = other.m_probe;
            size_t element_size = m_bucket_count * max_bucket_size + 1;
            m_buckets = allocator::allocate(element_size * sizeof(node_type));
            for (size_t i = 0; i < element_size; i++)
                new (m_buckets + i) node_type();

            for (size_t i = 0; i < element_size; i++)
                m_buckets[i] = other.m_buckets[i];

            // end flag
            m_buckets[element_size - 1].flag = ElementFlag::e_end;
        }

        return *this;
    }

    Hash& operator=(Hash&& other) noexcept
    {
        swap(other);
        return *this;
    }

    Iterator insert(value_type&& value)
    {
        size_t probe_group = m_probe(value_hash()(value), m_bucket_count);
        size_t i = 0;
        for (; i < max_bucket_size; i++)
        {
            if (m_buckets[probe_group * max_bucket_size + i].flag == ElementFlag::e_invalid)
            {
                m_buckets[probe_group * max_bucket_size + i].flag = ElementFlag::e_valid;
                m_buckets[probe_group * max_bucket_size + i].val = value;
                m_size++;
                return Iterator(m_buckets + probe_group * max_bucket_size + i);
            }
        }

        rehash(m_bucket_count * 2);
        return insert(value);
    }

    Iterator insert(const value_type& value)
    {
        size_t probe_group = m_probe(value_hash()(value), m_bucket_count);
        size_t i = 0;
        for (; i < max_bucket_size; i++)
        {
            if (m_buckets[probe_group * max_bucket_size + i].flag == ElementFlag::e_invalid)
            {
                m_buckets[probe_group * max_bucket_size + i].flag = ElementFlag::e_valid;
                m_buckets[probe_group * max_bucket_size + i].val = value;
                m_size++;
                return Iterator(m_buckets + probe_group * max_bucket_size + i);
            }
        }

        rehash(m_bucket_count * 2);
        return insert(value);
    }

    template <typename... Args>
        requires(std::is_constructible_v<value_type, Args...>)
    Iterator emplace(Args&&... args)
    {
        value_type val(std::forward<Args>(args)...);
        return insert(std::move(val));
    }

    void erase(const key_type& key)
    {
        node_type* node = find_node(key);
        if (node != nullptr)
        {
            node->flag = ElementFlag::e_invalid;
            m_size--;
        }
    }

    void erase(Iterator&& iter)
    {
        if (iter.m_node != nullptr && iter.m_node->flag == ElementFlag::e_valid)
        {
            iter.m_node->flag = ElementFlag::e_invalid;
            m_size--;
        }
    }

    void erase(const Iterator& iter)
    {
        if (iter.m_node != nullptr && iter.m_node->flag == ElementFlag::e_valid)
        {
            iter.m_node->flag = ElementFlag::e_invalid;
            m_size--;
        }
    }

    void rehash(size_t new_bucket_count)
    {
        CONTAINER_ASSERT((new_bucket_count & new_bucket_count - 1) == 0, "new bucket count must be power of 2!");

        size_t capacity = m_bucket_count * max_bucket_size;
        if (capacity * max_load_factor_numerator <= m_size * max_load_factor_denominator)
        {
            size_t rehash_bucket_count = std::max(m_bucket_count * 2, new_bucket_count);
            size_t rehash_capacity = rehash_bucket_count * max_bucket_size + 1;
            node_type* new_buckets = allocator::allocate(rehash_capacity * sizeof(node_type));

            m_probe.increment();
            for (size_t i = 0; i < capacity; i++)
            {
                if (m_buckets[i].flag == ElementFlag::e_valid)
                {
                    size_t index = m_probe(value_hash()(m_buckets[i].val), rehash_bucket_count);
                    size_t j = 0;
                    for (; j < max_bucket_size; j++)
                    {
                        if (new_buckets[index * max_bucket_size + j].flag == ElementFlag::e_invalid)
                        {
                            new_buckets[index * max_bucket_size + j] = m_buckets[i];
                            break;
                        }
                    }
                    if (j == max_bucket_size)
                        CONTAINER_LOG_ERROR("no suitable position to rehash!");
                }
            }
            allocator::deallocate(m_buckets);
            m_buckets = new_buckets;
            m_bucket_count = new_bucket_count;
        }
        else if (new_bucket_count * max_bucket_size * max_load_factor_numerator > m_size * max_load_factor_denominator)
        {
            size_t rehash_bucket_count = new_bucket_count;
            size_t rehash_capacity = rehash_bucket_count * max_bucket_size + 1;
            node_type* new_buckets = allocator::allocate(rehash_capacity * sizeof(node_type));

            m_probe.decrement();
            for (size_t i = 0; i < capacity; i++)
            {
                if (m_buckets[i].flag == ElementFlag::e_valid)
                {
                    size_t index = m_probe(value_hash()(m_buckets[i].val), rehash_bucket_count);
                    size_t j = 0;
                    for (; j < max_bucket_size; j++)
                    {
                        if (new_buckets[index * max_bucket_size + j].flag == ElementFlag::e_invalid)
                        {
                            new_buckets[index * max_bucket_size + j] = m_buckets[i];
                            break;
                        }
                    }
                    if (j == max_bucket_size)
                        CONTAINER_LOG_ERROR("no suitable position to rehash!");
                }
            }
            allocator::deallocate(m_buckets);
            m_buckets = new_buckets;
            m_bucket_count = new_bucket_count;
        }
    }

    NODISCARD bool empty() const
    {
        return m_size == 0;
    }

    NODISCARD size_t size() const
    {
        return m_size;
    }

    Iterator begin()
    {
        for (size_t i = 0; i < m_bucket_count * max_bucket_size + 1; i++)
            if (m_buckets[i].flag == ElementFlag::e_valid)
                return Iterator(m_buckets + i);
        return end();
    }

    Iterator const begin() const
    {
        for (size_t i = 0; i < m_bucket_count * max_bucket_size + 1; i++)
            if (m_buckets[i].flag == ElementFlag::e_valid)
                return Iterator(m_buckets + i);
        return end();
    }

    Iterator end()
    {
        return Iterator(m_buckets + m_bucket_count * max_bucket_size);
    }

    Iterator const end() const
    {
        return Iterator(m_buckets + m_bucket_count * max_bucket_size);
    }

    void swap(Hash& other) noexcept
    {
        Amazing::swap(m_buckets, other.m_buckets);
        Amazing::swap(m_bucket_count, other.m_bucket_count);
        Amazing::swap(m_probe, other.m_probe);
        Amazing::swap(m_size, other.m_size);
    }

protected:
    node_type* find_node(const key_type& key) const
    {
        size_t probe_group = m_probe(key_hash()(key), m_bucket_count);
        for (size_t i = 0; i < max_bucket_size; i++)
        {
            if (m_buckets[probe_group * max_bucket_size + i].flag == ElementFlag::e_valid &&
                key_equal()(Trait::key_func(m_buckets[probe_group * max_bucket_size + i].val), key))
                return m_buckets + probe_group * max_bucket_size + i;
        }

        return m_buckets + m_bucket_count * max_bucket_size;
    }

private:
    node_type* m_buckets;
    size_t m_bucket_count;
    size_t m_size;
    Probe m_probe;
};


INTERNAL_NAMESPACE_END

AMAZING_NAMESPACE_END

#endif //HASH_H
