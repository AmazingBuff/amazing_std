//
// Created by AmazingBuff on 25-7-21.
//

#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "astd/sync/thread/thread.h"
#include "astd/container/vector.h"
#include "astd/container/queue.h"
#include "astd/memory/pointer.h"
#include <mutex>

AMAZING_NAMESPACE_BEGIN

class Task;
class TaskGraph;
class Executor;

class Worker final : public Thread
{
public:
    explicit Worker(Executor* executor);

    Worker(Worker&&) = default;
    Worker& operator=(Worker&&) = default;
private:
    void run(std::stop_token token) override;
private:
    Executor* m_ref_executor;
};

class Executor
{
public:
    explicit Executor(size_t thread_count);
    ~Executor();

    void run(TaskGraph& graph);
    void wait();

    Executor(const Executor&) = delete;
    Executor& operator=(const Executor&) = delete;
    Executor(Executor&&) = delete;
    Executor& operator=(Executor&&) = delete;
private:
    void insert_task(Task* task);
    Task* fetch_task();
private:
    Vector<Worker*> m_worker_pool;
    Vector<Task*> m_task_pool;
    std::mutex m_task_mutex;
    uint32_t m_task_index;

    std::atomic<uint32_t> m_counter;

    friend class Worker;
};

AMAZING_NAMESPACE_END
#endif //EXECUTOR_H
