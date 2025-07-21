//
// Created by AmazingBuff on 25-7-19.
//

#ifndef TASK_H
#define TASK_H

#include "astd/container/vector.h"
#include "astd/trait/functional.h"
#include "astd/algorithm/iter.h"

AMAZING_NAMESPACE_BEGIN

class TaskGraph;

class Task
{
public:
    explicit Task(Functional<void()> const& task) : m_task(task) {}

    template <typename F, typename... Args>
    explicit Task(F&& f, Args&&... args) : m_task([&] { f(args...); }) {}

    ~Task() = default;

    template <typename... Ts>
    Task& precede(Ts&&... tasks)
    {
        (m_precede_nodes.push_back(std::forward<Ts>(tasks)), ...);
        (tasks->m_succeed_nodes.push_back(this), ...);
        return *this;
    }

    template <typename... Ts>
    Task& succeed(Ts&&... tasks)
    {
        (tasks->precede(this), ...);
        return *this;
    }

    void operator()()
    {
        m_task();
    }

    NODISCARD explicit operator bool() const
    {
        return static_cast<bool>(m_task);
    }

private:
    Functional<void()> m_task;
    Vector<Task*> m_precede_nodes;
    Vector<Task*> m_succeed_nodes;
    std::atomic<uint32_t> m_join_counter;

    friend class TaskGraph;
    friend class Worker;
};


class TaskGraph
{
public:
    TaskGraph();
    ~TaskGraph();

    template <typename F, typename... Args>
    Task* emplace(F&& f, Args&&... args)
    {
        Task* task = PLACEMENT_NEW(Task, sizeof(Task), std::forward<F>(f), std::forward<Args>(args)...);
        m_task_nodes.push_back(task);
        return task;
    }

    void erase(Task* task);
private:
    // reorder task nodes based on their dependencies
    // after compile, all no dependency node will be moved to the front of the task graph
    void compile();
private:
    Vector<Task*> m_task_nodes;
    uint32_t m_join_counter;

    friend class Executor;
};

AMAZING_NAMESPACE_END
#endif //TASK_H
