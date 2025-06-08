//
// Created by AmazingBuff on 2025/5/19.
//

#ifndef SORT_H
#define SORT_H

#include "astd/base/define.h"
#include "astd/base/logger.h"
#include "astd/base/util.h"
#include "astd/trait/trait.h"
#include "astd/memory/allocator.h"

AMAZING_NAMESPACE_BEGIN

INTERNAL_NAMESPACE_BEGIN

template <typename Iter>
Iter iter_move(Iter ori, int64_t distance)
{
    while (distance > 0)
    {
        ++ori;
        --distance;
    }

    while (distance < 0)
    {
        --ori;
        ++distance;
    }

    return ori;
}

template <typename Iter, typename Pred>
Iter median_of_three(Iter first, Iter last, Pred pred)
{
    int64_t distance = last - first;
    Iter middle = iter_move(first, distance / 2);
    Iter prev_last = iter_move(last, -1);
    Iter ret;
    if (pred(*first, *middle))
    {
        if (pred(*middle, *prev_last))
            ret = middle;
        else
        {
            if (pred(*first, *prev_last))
                ret = prev_last;
            else
                ret = first;
        }
    }
    else
    {
        if (pred(*prev_last, *middle))
            ret = middle;
        else
        {
            if (pred(*first, *prev_last))
                ret = first;
            else
                ret = prev_last;
        }
    }
    return ret;
}

template <typename Iter, typename Pred>
void sort_unchecked(Iter first, Iter last, Pred pred)
{
    Iter prev_last = iter_move(last, -1);
    if (first == prev_last || first == last)
        return;

    Iter mid = median_of_three(first, last, pred);
    Amazing::swap(*first, *mid);

    Iter begin = first;
    Iter end = prev_last;
    while (begin != end)
    {
        while (!pred(*end, *begin) && begin != end)
            --end;

        Amazing::swap(*begin, *end);

        while (!pred(*end, *begin) && begin != end)
            ++begin;

        Amazing::swap(*begin, *end);
    }

    sort_unchecked(first, begin, pred);
    sort_unchecked(++end, last, pred);
}

template <typename Iter, typename Pred>
void make_heap(Iter first, Iter last, Pred pred)
{
    int64_t distance = last - first;
    Iter leaf = iter_move(first, distance / 2);
    while (leaf != first)
    {
        int64_t offset = leaf - first;
        Iter prev_leaf = iter_move(leaf, -1);
        Iter left = iter_move(prev_leaf, offset);
        Iter right = left;
        if (offset * 2 + 1 <= distance)
            right = iter_move(left, 1);

        if (pred(*prev_leaf, *right))
            Amazing::swap(*prev_leaf, *right);
        if (pred(*prev_leaf, *left))
            Amazing::swap(*prev_leaf, *left);
        leaf = prev_leaf;
    }
}

template <typename Iter, typename Pred>
void heap_sort_unchecked(Iter first, Iter last, Pred pred)
{
    Iter prev_last = iter_move(last, -1);
    if (first == prev_last)
        return;

    make_heap(first, last, pred);
    Amazing::swap(*first, *prev_last);
    heap_sort_unchecked(first, prev_last, pred);
}

INTERNAL_NAMESPACE_END

template <typename Container>
typename Container::Iterator begin(Container& container)
{
    return container.begin();
}

template <typename Container>
typename Container::Iterator end(Container& container)
{
    return container.end();
}

template <typename Tp, size_t N>
Tp* begin(Tp(&arr)[N])
{
    return arr;
}

template <typename Tp, size_t N>
Tp* end(Tp(&arr)[N])
{
    return arr + N;
}

template <typename Iter, typename Pred>
void sort(Iter begin, Iter end, Pred predicate)
{
    Internal::sort_unchecked(begin, end, predicate);
}

template <typename Iter>
void sort(Iter begin, Iter end)
{
    sort(begin, end, Less<typename Iter::value_type>());
}

template <typename Container>
void sort(Container& container)
{
    sort(begin(container), end(container));
}

template <typename Tp, size_t N>
void sort(Tp(&arr)[N])
{
    sort(begin(arr), end(arr), Less<Tp>());
}



template <typename Iter, typename Pred>
void heap_sort(Iter begin, Iter end, Pred predicate)
{
    Internal::heap_sort_unchecked(begin, end, predicate);
}

template <typename Iter>
void heap_sort(Iter begin, Iter end)
{
    heap_sort(begin, end, Less<typename Iter::value_type>());
}

template <typename Container>
void heap_sort(Container& container)
{
    heap_sort(begin(container), end(container));
}

template <typename Tp, size_t N>
void heap_sort(Tp(&arr)[N])
{
    heap_sort(begin(arr), end(arr), Less<Tp>());
}


AMAZING_NAMESPACE_END

#endif //SORT_H
