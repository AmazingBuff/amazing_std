//
// Created by AmazingBuff on 2025/6/3.
//

#ifndef STACK_H
#define STACK_H

#include "list.h"
#include "astd/base/except.h"

AMAZING_NAMESPACE_BEGIN
template<typename Tp, template <typename> typename Alloc = Allocator>
class Stack
{
    using allocator = Alloc<Internal::ListNode<Tp>>;
public:
    Stack() : m_size(0)
    {
        m_head = allocator::allocate(1);
        m_head->val = Tp();
        m_head->prev = nullptr;
        m_head->next = nullptr;
    }

    ~Stack()
    {
        clear();

        allocator::deallocate(m_head);
        m_head = nullptr;
    }

    template <typename... Args>
    void push(Args&&... args)
    {
        Tp node(std::forward<Args>(args)...);
        push(node);
    }

    void push(const Tp& value)
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

    void pop()
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

    NODISCARD bool empty() const
    {
        return m_size == 0;
    }

    NODISCARD size_t size() const
    {
        return m_size;
    }

    NODISCARD const Tp& top() const
    {
        if (!empty())
            return m_head->prev->val;

        throw AStdException(AStdError::NO_VALID_PARAMETER);
    }

    NODISCARD Tp& top()
    {
        if (!empty())
            return m_head->prev->val;

        throw AStdException(AStdError::NO_VALID_PARAMETER);
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

private:
    Internal::ListNode<Tp>* m_head;
    size_t m_size;
};

AMAZING_NAMESPACE_END
#endif //STACK_H
