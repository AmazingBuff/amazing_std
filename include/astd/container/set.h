#pragma once

#include "internal/tree.h"
#include "internal/hash.h"

AMAZING_NAMESPACE_BEGIN

INTERNAL_NAMESPACE_BEGIN

template <typename Tp, typename Pred, template <typename> typename Alloc, bool Multi>
class SetTrait
{
public:
    using key_type      =   Tp;
    using value_type    =   Tp;
    using key_compare   =   Pred;
    template <typename T>
    using alloc         =   Alloc<T>;
    using value_compare =   key_compare;

    static constexpr bool is_multi = Multi;

    // extract key from element value
    static const key_type& key_func(const value_type& val)
    {
        return val;
    }
};


template <typename Tp, typename Hash, typename Equal, template <typename> typename Alloc, bool Multi>
class HashSetTrait
{
public:
    using key_type      =   Tp;
    using value_type    =   Tp;
    using key_hash      =   Hash;
    using key_equal     =   Equal;
    using node_type     =   HashNode<value_type>;
    using allocator     =   Alloc<node_type>;
    using value_hash    =   key_hash;

    static constexpr bool is_multi = Multi;

    static constexpr size_t max_bucket_size = 4;
    static constexpr size_t max_load_factor_numerator = 7;
    static constexpr size_t max_load_factor_denominator = 8;

    // extract key from element value
    static const key_type& key_func(const value_type& val)
    {
        return val;
    }
};

INTERNAL_NAMESPACE_END

template <typename Tp, typename Pred = Less<Tp>, template <typename> typename Alloc = Allocator>
class Set : public Internal::RBTree<Internal::SetTrait<Tp, Pred, Alloc, false>>
{
    using Tree = Internal::RBTree<Internal::SetTrait<Tp, Pred, Alloc, false>>;
    using Iterator = typename Tree::Iterator;
public:
    Set() = default;

    template <typename Iter>
    Set(Iter first, Iter last)
    {
        Tree::insert(first, last);
    }

    Iterator find(const Tp& key)
    {
        return Iterator(Tree::find_node(key));
    }

    Iterator const find(const Tp& key) const
    {
        return Iterator(Tree::find_node(key));
    }
};

template <typename Tp, typename Pred = Less<Tp>, template <typename> typename Alloc = Allocator>
class MultiSet : public Internal::RBTree<Internal::SetTrait<Tp, Pred, Alloc, true>>
{
    using Tree = Internal::RBTree<Internal::SetTrait<Tp, Pred, Alloc, true>>;
    using Iterator = typename Tree::Iterator;
public:
    Iterator find(const Tp& key)
    {
        return Iterator(Tree::find_node(key));
    }

    Iterator const find(const Tp& key) const
    {
        return Iterator(Tree::find_node(key));
    }
};


// hash set
template <typename Tp, typename Hasher = std::hash<Tp>, typename Equal = Equal<Tp>, template <typename> typename Alloc = Allocator>
class HashSet : public Internal::Hash<Internal::HashSetTrait<Tp, Hasher, Equal, Alloc, false>>
{
    using Hash = Internal::Hash<Internal::HashSetTrait<Tp, Hasher, Equal, Alloc, false>>;
    using Iterator = typename Hash::Iterator;
public:
    Iterator find(const Tp& key)
    {
        return Iterator(Hash::find_node(key));
    }

    Iterator const find(const Tp& key) const
    {
        return Iterator(Hash::find_node(key));
    }
};

template <typename Tp, typename Hasher = std::hash<Tp>, typename Equal = Equal<Tp>, template <typename> typename Alloc = Allocator>
class MultiHashSet : public Internal::Hash<Internal::HashSetTrait<Tp, Hasher, Equal, Alloc, true>>
{
    using Hash = Internal::Hash<Internal::HashSetTrait<Tp, Hasher, Equal, Alloc, true>>;
    using Iterator = typename Hash::Iterator;
public:
    Iterator find(const Tp& key)
    {
        return Iterator(Hash::find_node(key));
    }

    Iterator const find(const Tp& key) const
    {
        return Iterator(Hash::find_node(key));
    }
};


AMAZING_NAMESPACE_END