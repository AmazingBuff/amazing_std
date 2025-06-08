#pragma once

#include "astd/base/define.h"
#include "astd/trait/trait.h"
#include "astd/memory/allocator.h"

AMAZING_NAMESPACE_BEGIN

INTERNAL_NAMESPACE_BEGIN

template<typename Tp>
struct ListNode
{
    Tp val;
    ListNode* prev;
    ListNode* next;
};

INTERNAL_NAMESPACE_END


template<typename Tp, template <typename> typename Alloc = Allocator>
class List
{
    using allocator = Alloc<Internal::ListNode<Tp>>;
public:
    class Iterator
    {
    public:
        Iterator() : m_ptr(nullptr) {}
        explicit Iterator(Internal::ListNode<Tp>* ptr) : m_ptr(ptr) {}

        Iterator& operator++()
        {
            m_ptr = m_ptr->next;
            return *this;
        }

        Iterator& operator--()
        {
            m_ptr = m_ptr->prev;
            return *this;
        }

        Tp* operator->()
        {
            return &m_ptr->val;
        }

        Tp& operator*()
        {
            return m_ptr->val;
        }

        const Tp& operator*() const
        {
            return m_ptr->val;
        }

        NODISCARD bool operator==(const Iterator& other)
        {
            return m_ptr == other.m_ptr;
        }

        NODISCARD bool operator!=(const Iterator& other)
        {
            return m_ptr != other.m_ptr;
        }
    private:
        Internal::ListNode<Tp>* m_ptr;
    };
public:
    List() : m_size(0)
    {
        m_head = allocator::allocate(1);
        m_head->val = Tp();
        m_head->prev = nullptr;
        m_head->next = nullptr;
    }

    ~List()
    {
        clear();

        allocator::deallocate(m_head);
        m_head = nullptr;
    }

    void insert(size_t index, const Tp& value)
    {
        Internal::ListNode<Tp>* node = allocator::allocate(1);
        node->val = value;

        Internal::ListNode<Tp>* p = m_head;
        if (index <= m_size / 2 + 1)
        {
            while (index != 0)
            {
                p = p->next;
                index--;
            }
        }
        else
        {
            index = m_size - index + 1;
            while (index != 0)
            {
                p = p->prev;
                index--;
            }
        }

        if (p->next != nullptr)
        {
            p->next->prev = node;
            node->next = p->next;
        }
        else
            m_head->prev = node;

        node->next = p->next;
        node->prev = p;
        p->next = node;

        m_size++;
    }

    void emplace_back(Tp&& value)
    {
        Internal::ListNode<Tp>* node = allocator::allocate(1);
        node->val = value;
        node->next = nullptr;

        if (!empty())
        {
            m_head->prev->next = node;
            node->prev = m_head->prev;
        }
        else
        {
            m_head->next = node;
            node->prev = m_head;
        }

        m_head->prev = node;
        m_size++;
    }

    void push_back(const Tp& value)
    {
        Internal::ListNode<Tp>* node = allocator::allocate(1);
        node->val = value;
        node->next = nullptr;

        if (!empty())
        {
            m_head->prev->next = node;
            node->prev = m_head->prev;
        }
        else
        {
            m_head->next = node;
            node->prev = m_head;
        }

        m_head->prev = node;
        m_size++;
    }

    void emplace_front(Tp&& value)
    {
        Internal::ListNode<Tp>* node = allocator::allocate(1);
        node->val = value;
        node->prev = m_head;

        if (!empty())
        {
            m_head->next->front = node;
            node->next = m_head->next;
        }
        else
        {
            m_head->next = node;
            node->next = nullptr;
        }

        m_head->next = node;
        m_size++;
    }

    void push_front(const Tp& value)
    {
        Internal::ListNode<Tp>* node = allocator::allocate(1);
        node->val = value;
        node->prev = m_head;

        if (!empty())
        {
            m_head->next->front = node;
            node->next = m_head->next;
        }
        else
        {
            m_head->next = node;
            node->next = nullptr;
        }

        m_head->next = node;
        m_size++;
    }

    void pop_back()
    {
        if (empty())
            return;

        Internal::ListNode<Tp>* node = m_head->prev;

        if (m_size > 1)
            m_head->prev = node->prev;
        else
        {
            m_head->prev = nullptr;
            m_head->next = nullptr;
        }

        allocator::deallocate(node);
        node = nullptr;

        m_size--;
    }

    void pop_front()
    {
        if (empty())
            return;

        Internal::ListNode<Tp>* node = m_head->next;

        if (m_size > 1)
            m_head->next = node->next;
        else
        {
            m_head->prev = nullptr;
            m_head->next = nullptr;
        }

        allocator::deallocate(node);
        node = nullptr;

        m_size--;
    }

    NODISCARD bool empty() const
    {
        return m_size == 0;
    }

    NODISCARD size_t size() const
    {
        return m_size;
    }

    NODISCARD Tp front() const
    {
        return m_head->next->val;
    }

    NODISCARD Tp back() const
    {
        return m_head->prev->val;
    }

    void clear()
    {
        Internal::ListNode<Tp>* node = m_head->next;
        while (node != nullptr)
        {
            Internal::ListNode<Tp>* next = node->next;

            allocator::deallocate(node);
            node = next;
        }
        m_head->next = nullptr;

        m_size = 0;
    }

    Iterator begin()
    {
        return Iterator(m_head->next);
    }

    Iterator end()
    {
        return Iterator(nullptr);
    }


private:
    Internal::ListNode<Tp>* m_head;
    size_t m_size;
};


AMAZING_NAMESPACE_END