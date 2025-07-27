//
// Created by AmazingBuff on 25-7-16.
//

#ifndef PARALLEL_H
#define PARALLEL_H

#include "astd/sync/task/task.h"
#include "astd/sync/task/executor.h"
#include "astd/algorithm/iter.h"

AMAZING_NAMESPACE_BEGIN

enum class ParallelStrategy
{
    e_parallel,
    e_sequential,
};

template <typename Container, typename F>
void for_each(ParallelStrategy strategy, Container const& container, F&& f)
{
    switch (strategy)
    {
    case ParallelStrategy::e_sequential:
        for_each(begin(container), end(container), std::forward<F>(f));
        break;
    case ParallelStrategy::e_parallel:
    {
        TaskGraph graph(container.size());
        for_each(begin(container), end(container), [&](auto&& item)
        {
            graph.emplace(std::forward<F>(f), item);
        });

        UniquePtr<Executor> executor = UniquePtr<Executor>(PLACEMENT_NEW(Executor, sizeof(Executor), std::thread::hardware_concurrency()));

        executor->run(graph);
        executor->wait();
    }
    }
}


AMAZING_NAMESPACE_END

#endif //PARALLEL_H
