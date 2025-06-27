#pragma once

#include "astd/container/queue.h"

AMAZING_NAMESPACE_BEGIN

INTERNAL_NAMESPACE_BEGIN

template <typename Trait, typename NodeType>
struct TreeTrait : Trait
{
    using Trait::key_type;
    using Trait::value_type;
    using Trait::key_compare;
    using Trait::value_compare;
    using node_type = NodeType;
    using allocator = typename Trait::template alloc<node_type>;
};

template <typename Trait>
class BinaryTree
{
protected:
    using key_type = typename Trait::key_type;
    using value_type = typename Trait::value_type;
    using key_compare = typename Trait::key_compare;
    using value_compare = typename Trait::value_compare;
    using node_type = typename Trait::node_type;
    using allocator = typename Trait::allocator;
public:
    class Iterator
    {
    public:
        Iterator() : m_node(nullptr) {}
        explicit Iterator(node_type* node) : m_node(node) {}

        Iterator& operator++()
        {
            if (m_node->right)
            {
                // find the left-most node
                m_node = m_node->right;
                while (m_node->left)
                    m_node = m_node->left;
            }
            else if (m_node->parent)
            {
                node_type* parent = m_node->parent;
                if (parent->left == m_node)
                    m_node = parent;
                else
                    m_node = nullptr;
            }
            else
                m_node = nullptr; // no parent, end

            return *this;
        }

        Iterator& operator--()
        {
            if (m_node->left)
            {
                m_node = m_node->left;
                // find the right-most node
                while (m_node->right)
                    m_node = m_node->right;
            }
            else if (m_node->parent)
            {
                node_type* parent = m_node->parent;
                if (parent->right == m_node)
                    m_node = parent;
                else
                    m_node = nullptr;
            }

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
    };
public:
    BinaryTree() : m_root(nullptr), m_size(0) {}

    virtual ~BinaryTree()
    {
        clear();
    }

    template <typename... Args>
        requires(std::is_constructible_v<value_type, Args...>)
    Iterator emplace(Args&&... args)
    {
        value_type val(std::forward<Args>(args)...);
        return insert(std::move(val));
    }

    template <typename Iter>
    void insert(Iter first, Iter last)
    {
        while (first != last)
        {
            emplace(*first);
            ++first;
        }
    }

    Iterator insert(value_type&& val)
    {
        // find position
        node_type* node = m_root;
        node_type* parent = nullptr;
        bool left_pos = false; // true with left, which false with right
        while (node)
        {
            parent = node;
            if (value_compare()(node->val, val))
            {
                node = node->right;
                left_pos = false;
            }
            else if (value_compare()(val, node->val))
            {
                node = node->left;
                left_pos = true;
            }
            else
            {
                // equal
                if constexpr (!Trait::is_multi)
                    return Iterator(node);
                else
                {
                    // insert to right subtree
                    node = node->right;
                    while (node)
                    {
                        parent = node;
                        node = node->left;
                    }
                }
            }
        }

        node = allocate_node(std::move(val), parent);

        if (m_root)
        {
            if (left_pos)
                parent->left = node;
            else
                parent->right = node;
        }
        else
            m_root = node;

        insert_adjustment(node);

        m_size++;

        return Iterator(node);
    }

    bool erase(const key_type& key)
    {
        node_type* node = find_node(key);

        if (node == nullptr)
            return false;

        erase_adjustment(node);
        m_size--;

        return true;
    }

    void erase_range(const key_type& key)
    {
        while (node_type* node = find_node(key))
        {
            erase_adjustment(node);
            m_size--;
        }
    }

    void erase(Iterator&& it)
    {
        if (it.m_node)
        {
            erase_adjustment(it.m_node);
            m_size--;
        }
    }

    NODISCARD size_t size() const
    {
        return m_size;
    }

    NODISCARD bool empty() const
    {
        return m_size == 0;
    }

    NODISCARD size_t count(const key_type& key) const
    {
        if (node_type* node = find_node(key))
        {
            if constexpr (!Trait::is_multi)
                return 1;
            else
            {
                Iterator it(node);
                key_type node_key = Trait::key_func(*it);
                uint32_t count = 0;
                while (!key_compare()(node_key, key) && !key_compare()(key, node_key))
                {
                    count++;
                    ++it;
                }
                return count;
            }
        }
        return 0;
    }

    void clear()
    {
        Queue<node_type*> queue;
        if (m_root)
            queue.enqueue(std::move(m_root));
        while (!queue.empty())
        {
            node_type* front = queue.front();
            queue.dequeue();
            if (front->left)
                queue.enqueue(std::move(front->left));
            if (front->right)
                queue.enqueue(std::move(front->right));
            erase_directly(front);
        }

        m_root = nullptr;
        m_size = 0;
    }

    Iterator begin()
    {
        // left most
        node_type* node = m_root;
        if (node)
        {
            while (node->left)
                node = node->left;
        }
        return Iterator(node);
    }

    Iterator end()
    {
        return Iterator();
    }

    Iterator const begin() const
    {
        // left most
        node_type* node = m_root;
        if (node)
        {
            while (node->left)
                node = node->left;
        }
        return Iterator(node);
    }

    Iterator const end() const
    {
        return Iterator();
    }
protected:
    virtual void insert_adjustment(node_type* node) = 0;
    virtual void erase_adjustment(node_type* node) = 0;
    virtual node_type* allocate_node(value_type&& val, node_type* parent) = 0;

    node_type* left_rotate(node_type* node)
    {
        node_type* right = node->right;
        node->right = right->left;

        if (right->left)
            right->left->parent = node;

        right->left = node;
        right->parent = node->parent;

        if (node->parent)
        {
            if (node == node->parent->left)
                node->parent->left = right;
            else
                node->parent->right = right;
        }

        node->parent = right;

        return right;
    }

    node_type* right_rotate(node_type* node)
    {
        node_type* left = node->left;
        node->left = left->right;

        if (left->right)
            left->right->parent = node;

        left->right = node;
        left->parent = node->parent;

        if (node->parent)
        {
            if (node == node->parent->left)
                node->parent->left = left;
            else
                node->parent->right = left;
        }

        node->parent = left;

        return left;
    }

    void swap(node_type* s1, node_type* s2)
    {
        if (s1 == s2)
            return;

        node_type* s1_parent = s1->parent;
        node_type* s2_parent = s2->parent;

        if (s1_parent)
        {
            if (s1 == s1_parent->left)
                s1_parent->left = s2;
            else
                s1_parent->right = s2;
        }

        if (s2_parent)
        {
            if (s2 == s2_parent->left)
                s2_parent->left = s1;
            else
                s2_parent->right = s1;
        }

        s1->parent = s2_parent;
        s2->parent = s1_parent;

        if (s1->left)
            s1->left->parent = s2;
        if (s2->left)
            s2->left->parent = s1;

        if (s1->right)
            s1->right->parent = s2;
        if (s2->right)
            s2->right->parent = s1;

        node_type* left = s1->left;
        s1->left = s2->left;
        s2->left = left;

        node_type* right = s1->right;
        s1->right = s2->right;
        s2->right = right;
    }

    // find node for rb tree or find first node for multi rb tree
    node_type* find_node(const key_type& key) const
    {
        node_type* node = m_root;
        while (node)
        {
            const key_type& node_key = Trait::key_func(node->val);
            if (key_compare()(node_key, key))
                node = node->right;
            else if (key_compare()(key, node_key))
                node = node->left;
            else
            {
                if constexpr (!Trait::is_multi)
                    break;
                else
                {
                    if (node->left)
                        node = node->left;
                    else
                        break;
                }
            }
        }

        return node;
    }

    void erase_directly(node_type* node)
    {
        allocator::deallocate(node);
    }
protected:
    node_type* m_root;
    size_t      m_size;
};


template <typename Tp>
struct RBTreeNode
{
    Tp val;
    RBTreeNode* parent;
    RBTreeNode* left;
    RBTreeNode* right;

    enum class Color : bool
    {
        Red,
        Black
    } color;
};

template <typename Trait>
class RBTree : public BinaryTree<TreeTrait<Trait, RBTreeNode<typename Trait::value_type>>>
{
    using Tree = BinaryTree<TreeTrait<Trait, RBTreeNode<typename Trait::value_type>>>;

    using typename Tree::key_type;
    using typename Tree::value_type;
    using typename Tree::key_compare;
    using typename Tree::value_compare;
    using typename Tree::node_type;
    using typename Tree::allocator;

    static constexpr bool is_set = std::is_same_v<key_type, value_type>;
public:
    RBTree() = default;
    ~RBTree() override = default;
private:
    // node is parent
    void ll(node_type* node)
    {
        node_type* root = Tree::right_rotate(node);
        root->color = node->color;
        root->left->color = node_type::Color::Black;
        root->right->color = node_type::Color::Black;
    }

    void lr(node_type* node)
    {
        node_type* brother = node->left;
        Tree::left_rotate(brother);
        node_type* root = Tree::right_rotate(node);
        root->color = node->color;
        root->left->color = node_type::Color::Black;
        root->right->color = node_type::Color::Black;
    }

    void rl(node_type* node)
    {
        node_type* brother = node->right;
        Tree::right_rotate(brother);
        node_type* root = Tree::left_rotate(node);
        root->color = node->color;
        root->left->color = node_type::Color::Black;
        root->right->color = node_type::Color::Black;
    }

    void rr(node_type* node)
    {
        node_type* root = Tree::left_rotate(node);
        root->color = node->color;
        root->left->color = node_type::Color::Black;
        root->right->color = node_type::Color::Black;
    }

    // double black, node is black, and no child
    void double_black_adjustment(node_type* node)
    {
        if (node->parent == nullptr)
            return;

        node_type* parent = node->parent;
        node_type* brother = nullptr;
        bool left_pos = false;
        if (node == parent->left)
        {
            left_pos = true;
            brother = parent->right;
        }
        else
        {
            left_pos = false;
            brother = parent->left;
        }

        if (brother->color == node_type::Color::Black)
        {
            if (brother->left && brother->left->color == node_type::Color::Red &&
                brother->right && brother->right->color == node_type::Color::Red)
            {
                if (left_pos)
                    rr(parent);
                else
                    ll(parent);
            }
            else if (brother->left && brother->left->color == node_type::Color::Red)
            {
                if (left_pos)
                    rl(parent);
                else
                    ll(parent);
            }
            else if (brother->right && brother->right->color == node_type::Color::Red)
            {
                if (left_pos)
                    rr(parent);
                else
                    lr(parent);
            }
            else
            {
                // no red child
                brother->color = node_type::Color::Red;
                if (parent->color == node_type::Color::Red)
                    parent->color = node_type::Color::Black;
                else
                    double_black_adjustment(parent);
            }
        }
        else
        {
            brother->color = node_type::Color::Black;
            parent->color = node_type::Color::Red;
            if (left_pos)
            {
                // brother is right
                Tree::left_rotate(parent);
                double_black_adjustment(node);
            }
            else
            {
                // brother is left
                Tree::right_rotate(parent);
                double_black_adjustment(node);
            }
        }
    }

    // node has been found
    void erase_adjustment(node_type* node) override
    {
        if (node->left != nullptr && node->right != nullptr)
        {
            // two children
            node_type* left_most = node->right;
            while (left_most->left)
                left_most = left_most->left;

            Tree::swap(node, left_most);
            Amazing::swap(node->color, left_most->color);
            if (node == Tree::m_root)
                Tree::m_root = left_most;
            erase_adjustment(node);
        }
        else if (node->left != nullptr || node->right != nullptr)
        {
            // one child, this child must be red
            node_type* child = node->left ? node->left : node->right;
            child->parent = node->parent;
            child->color = node_type::Color::Black;
            if (node == Tree::m_root)
            {
                Tree::erase_directly(node);
                node = nullptr;
                Tree::m_root = child;
            }
            else
            {
                node_type* parent = node->parent;
                if (node == parent->left)
                    parent->left = child;
                else
                    parent->right = child;
                Tree::erase_directly(node);
                node = nullptr;
            }
        }
        else
        {
            if (node->color == node_type::Color::Red)
            {
                node_type* parent = node->parent;
                if (node == parent->left)
                    parent->left = nullptr;
                else
                    parent->right = nullptr;

                Tree::erase_directly(node);
                node = nullptr;
            }
            else
            {
                // if node is black, it must have a brother exclude root
                if (node == Tree::m_root)
                    Tree::m_root = nullptr;
                else
                {
                    double_black_adjustment(node);
                    node_type* parent = node->parent;

                    if (parent->left == node)
                        parent->left = nullptr;
                    else
                        parent->right = nullptr;

                    while (parent->parent != nullptr)
                        parent = parent->parent;

                    Tree::m_root = parent;
                }

                Tree::erase_directly(node);
                node = nullptr;
            }
        }
    }


    // node has been inserted to appropriate position
    void insert_adjustment(node_type* node) override
    {
        if (node == Tree::m_root)
        {
            node->color = node_type::Color::Black;
            return;
        }

        node_type* parent = node->parent;
        if (parent->color == node_type::Color::Red)
        {
            // root node can't be red, so parent can't be root, which means parent must have parent
            node_type* grand = parent->parent;

            // parent red while uncle black
            if (grand->left == nullptr || grand->right == nullptr ||
                grand->left->color == node_type::Color::Black ||
                grand->right->color == node_type::Color::Black)
            {
                node_type* adjust = nullptr;
                if (parent->left == node)
                {
                    if (grand->left == parent)
                    {
                        grand->color = parent->color;
                        parent->color = node_type::Color::Black;
                        adjust = Tree::right_rotate(grand);
                    }
                    else
                    {
                        parent = Tree::right_rotate(parent);
                        grand->color = parent->color;
                        parent->color = node_type::Color::Black;
                        adjust = Tree::left_rotate(grand);
                    }
                }
                else
                {
                    if (grand->left == parent)
                    {
                        parent = Tree::left_rotate(parent);
                        grand->color = parent->color;
                        parent->color = node_type::Color::Black;
                        adjust = Tree::right_rotate(grand);
                    }
                    else
                    {
                        grand->color = parent->color;
                        parent->color = node_type::Color::Black;
                        adjust = Tree::left_rotate(grand);
                    }
                }

                if (adjust->parent == nullptr)
                    Tree::m_root = adjust;
            }
            else
            {
                // both parent and uncle are red
                grand->left->color = node_type::Color::Black;
                grand->right->color = node_type::Color::Black;
                grand->color = node_type::Color::Red;

                insert_adjustment(grand);
            }
        }
    }

    node_type* allocate_node(value_type&& val, node_type* parent) override
    {
        node_type* node = allocator::allocate(1);
        node->val = val;
        node->parent = parent;
        node->left = nullptr;
        node->right = nullptr;
        node->color = node_type::Color::Red;

        return node;
    }
};


template <typename Tp>
struct AVLTreeNode
{
    Tp val;
    AVLTreeNode* parent;
    AVLTreeNode* left;
    AVLTreeNode* right;
    int32_t depth;
};

template <typename Trait>
class AVLTree : public BinaryTree<TreeTrait<Trait, AVLTreeNode<typename Trait::value_type>>>
{
    using Tree = BinaryTree<TreeTrait<Trait, AVLTreeNode<typename Trait::value_type>>>;

    using typename Tree::key_type;
    using typename Tree::value_type;
    using typename Tree::key_compare;
    using typename Tree::value_compare;
    using typename Tree::node_type;
    using typename Tree::allocator;
public:
    AVLTree() = default;
    ~AVLTree() override = default;
private:
    static int32_t balance_factor(node_type* node)
    {
        int32_t left_depth = node->left ? node->left->depth : 0;
        int32_t right_depth = node->right ? node->right->depth : 0;
        return left_depth - right_depth;
    }

    static int32_t update_depth(node_type* node)
    {
        int32_t left_depth = node->left ? node->left->depth : 0;
        int32_t right_depth = node->right ? node->right->depth : 0;
        node->depth = std::max(left_depth, right_depth) + 1;
        return node->depth;
    }

    static node_type* update_balance_factor(node_type* node)
    {
        while (node)
        {
            int32_t balance = balance_factor(node);
            if (std::abs(balance) > 1 || balance == 0)
                break;

            node_type* parent = node->parent;
            if (parent)
                update_depth(parent);

            node = parent;
        }
        return node;
    }

    // node is parent
    node_type* ll(node_type* node)
    {
        node_type* root = Tree::right_rotate(node);
        update_depth(root->right);
        root->depth = std::max(root->left->depth, root->right->depth) + 1;
        return root;
    }

    node_type* lr(node_type* node)
    {
        node_type* brother = node->left;
        Tree::left_rotate(brother);
        node_type* root = Tree::right_rotate(node);

        update_depth(root->left);
        update_depth(root->right);
        root->depth = std::max(root->left->depth, root->right->depth) + 1;
        return root;
    }

    node_type* rl(node_type* node)
    {
        node_type* brother = node->right;
        Tree::right_rotate(brother);
        node_type* root = Tree::left_rotate(node);

        update_depth(root->left);
        update_depth(root->right);
        root->depth = std::max(root->left->depth, root->right->depth) + 1;
        return root;
    }

    node_type* rr(node_type* node)
    {
        node_type* root = Tree::left_rotate(node);
        update_depth(root->left);
        root->depth = std::max(root->left->depth, root->right->depth) + 1;
        return root;
    }

    // node has been inserted to appropriate position
    void insert_adjustment(node_type* node) override
    {
        if (node == Tree::m_root)
            return;

        node_type* parent = node->parent;
        int32_t balance = balance_factor(parent);
        if (parent->left == node)
        {
            if (balance == 0)
            {
                parent->depth += 1;
                if (node_type* new_node = update_balance_factor(parent))
                {
                    balance = balance_factor(new_node);
                    if (balance == 2)
                    {
                        if (new_node->left == parent)
                            // all in left
                            ll(new_node);
                        else
                            lr(new_node);
                    }
                    else if (balance == -2)
                        rl(new_node);
                }
            }
        }
        else
        {
            if (balance == 0)
            {
                parent->depth += 1;
                if (node_type* new_node = update_balance_factor(parent))
                {
                    balance = balance_factor(new_node);
                    if (balance == 2)
                        lr(new_node);
                    else if (balance == -2)
                    {
                        if (new_node->right == parent)
                            // all in right
                            rr(new_node);
                        else
                            rl(new_node);
                    }
                }
            }
        }
    }

    // node mustn't be nullptr
    void erase_balance(node_type* node)
    {
        int32_t left_depth = node->left ? node->left->depth : 0;
        int32_t right_depth = node->right ? node->right->depth : 0;
        int32_t balance = left_depth - right_depth;
        if (balance >= 2)
        {
            balance = balance_factor(node->left);
            if (balance == 0)
                ll(node);
            else if (balance == -1)
            {
                node = lr(node);
                if (node_type* new_node = update_balance_factor(node->parent))
                    erase_balance(new_node);
            }
            else if (balance == 1)
            {
                node = ll(node);
                if (node_type* new_node = update_balance_factor(node->parent))
                    erase_balance(new_node);
            }
        }
        else if (balance <= -2)
        {
            balance = balance_factor(node->right);
            if (balance == 0)
                rr(node);
            else if (balance == -1)
            {
                node = rr(node);
                if (node_type* new_node = update_balance_factor(node->parent))
                    erase_balance(new_node);
            }
            else if (balance == 1)
            {
                node = rl(node);
                if (node_type* new_node = update_balance_factor(node->parent))
                    erase_balance(new_node);
            }
        }
    }

    void erase_adjustment(node_type* node) override
    {
        if (node->left != nullptr && node->right != nullptr)
        {
            // two children
            node_type* left_most = node->right;
            while (left_most->left)
                left_most = left_most->left;

            Tree::swap(node, left_most);
            if (node == Tree::m_root)
                Tree::m_root = left_most;
            erase_adjustment(node);
        }
        else if (node->left != nullptr || node->right != nullptr)
        {
            // one child
            node_type* child = node->left ? node->left : node->right;
            Tree::swap(node, child);
            if (node == Tree::m_root)
                Tree::m_root = child;
            erase_adjustment(node);
        }
        else
        {
            // no child
            if (node == Tree::m_root)
                Tree::m_root = nullptr;
            else
            {
                node_type* parent = node->parent;
                int32_t balance = balance_factor(parent);
                if (balance == 0)
                {
                    if (parent->left == node)
                        parent->left = nullptr;
                    else
                        parent->right = nullptr;
                }
                else if (balance == -1)
                {
                    if (parent->left == node)
                    {
                        parent->left = nullptr;
                        erase_balance(parent);
                    }
                    else
                    {
                        parent->right = nullptr;
                        if (parent->parent)
                            erase_balance(parent->parent);
                    }
                }
                else
                {
                    if (parent->left == node)
                    {
                        parent->left = nullptr;
                        if (parent->parent)
                            erase_balance(parent->parent);
                    }
                    else
                    {
                        parent->right = nullptr;
                        erase_balance(parent);
                    }
                }
            }

            Tree::erase_directly(node);
            node = nullptr;
        }
    }

    node_type* allocate_node(value_type&& val, node_type* parent) override
    {
        node_type* node = allocator::allocate(1);
        node->val = val;
        node->parent = parent;
        node->left = nullptr;
        node->right = nullptr;
        node->depth = 1;

        return node;
    }

};

INTERNAL_NAMESPACE_END

AMAZING_NAMESPACE_END