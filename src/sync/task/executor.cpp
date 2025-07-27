//
// Created by AmazingBuff on 25-7-21.
//

#include <astd/sync/task/executor.h>
#include <astd/sync/task/task.h>

AMAZING_NAMESPACE_BEGIN

Worker::Worker(Executor* executor) : m_ref_executor(executor) {}


void Worker::run(std::stop_token token)
{
    while(true)
    {
        if (token.stop_requested())
            return;

        if (Task* task = m_ref_executor->fetch_task())
        {
            task->operator()();
            for_each(task->m_succeed_nodes, [this](Task* succeed_node)
            {
                uint32_t counter = --succeed_node->m_join_counter;
                if (counter == 0)
                    m_ref_executor->insert_task(succeed_node);
            });
            --m_ref_executor->m_counter;
        }
    }
}


Executor::Executor(size_t thread_count) : m_task_index(0)
{
    ASSERT(thread_count > 0 && thread_count <= std::thread::hardware_concurrency(), "astd", "thread count must be in range [1, {}]!", std::thread::hardware_concurrency());

    m_worker_pool.reserve(thread_count);
    for (size_t i = 0; i < thread_count; ++i)
    {
        m_worker_pool.push_back(PLACEMENT_NEW(Worker, sizeof(Worker), this));
        m_worker_pool.back()->start();
    }
}

Executor::~Executor()
{
    for (Worker* worker : m_worker_pool)
    {
        if (worker->is_running())
            worker->stop();
        PLACEMENT_DELETE(Worker, worker);
    }
    m_worker_pool.clear();
}

void Executor::run(TaskGraph& graph)
{
    m_counter = graph.m_task_nodes.size();
    m_task_pool.reserve(graph.m_task_nodes.size());
    graph.compile();
    {
        std::lock_guard<std::mutex> lock(m_task_mutex);
        for (uint32_t i = 0; i < graph.m_join_counter; ++i)
            m_task_pool.push_back(graph.m_task_nodes[i]);
    }
}

void Executor::wait()
{
    while (m_counter.load(std::memory_order_acquire)) {}
    m_task_pool.clear();
    m_task_index = 0;
}

void Executor::insert_task(Task* task)
{
    std::lock_guard<std::mutex> lock(m_task_mutex);
    m_task_pool.push_back(task);
}

Task* Executor::fetch_task()
{
    std::lock_guard<std::mutex> lock(m_task_mutex);
    if (m_task_index < m_task_pool.size())
    {
        Task* task = m_task_pool[m_task_index];
        m_task_index++;
        return task;
    }

    return nullptr;
}

AMAZING_NAMESPACE_END