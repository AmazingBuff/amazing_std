#pragma once

#include "internal/tree.h"
#include "internal/hash.h"

AMAZING_NAMESPACE_BEGIN
INTERNAL_NAMESPACE_BEGIN

template <typename Key, typename Tp, typename Pred, template <typename> typename Alloc, bool Multi>
class MapTrait
{
public:
    using key_type      =   Key;
    using value_type    =   Pair<Key, Tp>;
    using key_compare   =   Pred;
    using node_type     =   RBTreeNode<value_type>;
    using allocator     =   Alloc<node_type>;
    
    class value_compare
    {
    public:
        NODISCARD bool operator()(const value_type& lhs, const value_type& rhs) const 
        {
            return key_compare()(lhs.first, rhs.first);
        }
    };

    static constexpr bool is_multi = Multi;

    // extract key from element value
    static const key_type& key_func(const value_type& val)
    {
        return val.first;
    }
};



template <typename Key, typename Tp, typename Hash, typename Equal, template <typename> typename Alloc, bool Multi>
class HashMapTrait
{
public:
    using key_type      =   Key;
    using value_type    =   Pair<Key, Tp>;
    using key_hash      =   Hash;
    using key_equal     =   Equal;
    using node_type     =   HashNode<value_type>;
    using allocator     =   Alloc<node_type>;

    class value_hash
    {
    public:
        NODISCARD size_t operator()(const value_type& value) const
        {
            return key_hash()(value.first);
        }
    };

    static constexpr bool is_multi = Multi;

    static constexpr size_t max_bucket_size = 4;
    static constexpr size_t max_load_factor_numerator = 7;
    static constexpr size_t max_load_factor_denominator = 8;

    // extract key from element value
    static const key_type& key_func(const value_type& val)
    {
        return val.first;
    }
};


INTERNAL_NAMESPACE_END

template <typename Key, typename Tp, typename Pred = Less<Key>, template <typename> typename Alloc = Allocator>
class Map : public Internal::RBTree<Internal::MapTrait<Key, Tp, Pred, Alloc, false>>
{
    using Tree = Internal::RBTree<Internal::MapTrait<Key, Tp, Pred, Alloc, false>>;
    using Iterator = typename Tree::Iterator;
public:
    Tp& operator[](const Key& key)
    {
        auto node = Tree::find_node(key);
        if (node == nullptr)
            return Tree::emplace(key, Tp())->second;

        return node->val.second;
    }

    const Tp& operator[](const Key& key) const
    {
        return Tree::find_node(key)->val.second;
    }

    Iterator find(const Key& key)
    {
        return Iterator(Tree::find_node(key));
    }

    Iterator const find(const Key& key) const
    {
        return Iterator(Tree::find_node(key));
    }
};

template <typename Key, typename Tp, typename Pred = Less<Key>, template <typename> typename Alloc = Allocator>
class MultiMap : public Internal::RBTree<Internal::MapTrait<Key, Tp, Pred, Alloc, true>>
{
    using Tree = Internal::RBTree<Internal::MapTrait<Key, Tp, Pred, Alloc, true>>;
    using Iterator = typename Tree::Iterator;
public:
    Iterator find(const Key& key)
    {
        return Iterator(Tree::find_node(key));
    }

    Iterator const find(const Key& key) const
    {
        return Iterator(Tree::find_node(key));
    }
};

template <typename Key, typename Tp, typename Pred = Less<Key>, template <typename> typename Alloc = Allocator>
class BalancedMap : public Internal::AVLTree<Internal::MapTrait<Key, Tp, Pred, Alloc, false>>
{
    using Tree = Internal::AVLTree<Internal::MapTrait<Key, Tp, Pred, Alloc, false>>;
    using Iterator = typename Tree::Iterator;
public:
    Tp& operator[](const Key& key)
    {
        auto node = Tree::find_node(key);
        if (node == nullptr)
            return Tree::emplace(key, Tp())->second;

        return node->val.second;
    }

    const Tp& operator[](const Key& key) const
    {
        return Tree::find_node(key)->val.second;
    }

    Iterator find(const Key& key)
    {
        return Iterator(Tree::find_node(key));
    }

    Iterator const find(const Key& key) const
    {
        return Iterator(Tree::find_node(key));
    }
};

template <typename Key, typename Tp, typename Pred = Less<Key>, template <typename> typename Alloc = Allocator>
class BalancedMultiMap : public Internal::AVLTree<Internal::MapTrait<Key, Tp, Pred, Alloc, true>>
{
    using Tree = Internal::AVLTree<Internal::MapTrait<Key, Tp, Pred, Alloc, true>>;
    using Iterator = typename Tree::Iterator;
public:
    Iterator find(const Key& key)
    {
        return Iterator(Tree::find_node(key));
    }

    Iterator const find(const Key& key) const
    {
        return Iterator(Tree::find_node(key));
    }
};

// hash map
template <typename Key, typename Tp, typename Hasher = std::hash<Key>, typename Equal = Equal<Key>, template <typename> typename Alloc = Allocator>
class HashMap : public Internal::Hash<Internal::HashMapTrait<Key, Tp, Hasher, Equal, Alloc, false>>
{
    using Hash = Internal::Hash<Internal::HashMapTrait<Key, Tp, Hasher, Equal, Alloc, false>>;
    using Iterator = typename Hash::Iterator;
public:
    Tp& operator[](const Key& key)
    {
        auto node = Hash::find_node(key);
        if (Iterator(node) == Hash::end())
        {
            Tp val = Tp();
            return Hash::emplace(key, val)->second;
        }

        return node->val.second;
    }

    const Tp& operator[](const Key& key) const
    {
        return Hash::find_node(key)->val.second;
    }

    Iterator find(const Key& key)
    {
        return Iterator(Hash::find_node(key));
    }

    Iterator const find(const Key& key) const
    {
        return Iterator(Hash::find_node(key));
    }
};

template <typename Key, typename Tp, typename Hasher = std::hash<Key>, typename Equal = Equal<Key>, template <typename> typename Alloc = Allocator>
class MultiHashMap : public Internal::Hash<Internal::HashMapTrait<Key, Tp, Hasher, Equal, Alloc, true>>
{
    using Hash = Internal::Hash<Internal::HashMapTrait<Key, Tp, Hasher, Equal, Alloc, true>>;
    using Iterator = typename Hash::Iterator;
public:
    Iterator find(const Key& key)
    {
        return Iterator(Hash::find_node(key));
    }

    Iterator const find(const Key& key) const
    {
        return Iterator(Hash::find_node(key));
    }
};


AMAZING_NAMESPACE_END