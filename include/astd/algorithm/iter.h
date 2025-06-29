//
// Created by AmazingBuff on 2025/6/26.
//

#ifndef ITER_H
#define ITER_H

#include "sort.h"

AMAZING_NAMESPACE_BEGIN

template <typename Iter, typename F>
bool any_of(Iter first, Iter last, F&& f)
{
    for (; first != last; ++first)
    {
        if (f(*first))
            return true;
    }
    return false;
}

template <typename Container, typename F>
bool any_of(Container const& container, F&& f)
{
    return any_of(begin(container), end(container), std::forward<F>(f));
}

template <typename Iter, typename F>
bool all_of(Iter first, Iter last, F&& f)
{
    for (; first != last; ++first)
    {
        if (!f(*first))
            return false;
    }
    return true;
}

template <typename Container, typename F>
bool all_of(Container const& container, F&& f)
{
    return all_of(begin(container), end(container), std::forward<F>(f));
}

template <typename Iter, typename F>
void for_each(Iter first, Iter last, F&& f)
{
    for (; first != last; ++first)
        f(*first);
}

template <typename Container, typename F>
void for_each(Container const& container, F&& f)
{
    for_each(begin(container), end(container), std::forward<F>(f));
}

AMAZING_NAMESPACE_END

#endif //ITER_H
