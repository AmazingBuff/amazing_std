//
// Created by AmazingBuff on 25-7-21.
//

#include <astd/sync/task/task.h>

AMAZING_NAMESPACE_BEGIN

TaskGraph::TaskGraph(uint32_t task_count) : m_task_counter(0), m_join_counter(0)
{
    m_task_nodes.reserve(task_count);
}

TaskGraph::~TaskGraph()
{
    for (uint32_t i = 0; i < m_task_counter; ++i)
        PLACEMENT_DELETE(Task, m_task_nodes[i]);
}

void TaskGraph::erase(Task* task)
{
    // we don't care the order of nodes
    for (Task* node : task->m_precede_nodes)
    {
        for (uint32_t i = 0; i < node->m_succeed_nodes.size(); ++i)
        {
            if (node->m_succeed_nodes[i] == task)
            {
                node->m_succeed_nodes[i] = node->m_succeed_nodes.back();
                node->m_succeed_nodes.pop_back();
                break;
            }
        }
    }

    for (Task* node : task->m_succeed_nodes)
    {
        for (uint32_t i = 0; i < node->m_precede_nodes.size(); ++i)
        {
            if (node->m_precede_nodes[i] == task)
            {
                node->m_precede_nodes[i] = node->m_precede_nodes.back();
                node->m_precede_nodes.pop_back();
                break;
            }
        }
    }

    for (uint32_t i = 0; i < m_task_counter; ++i)
    {
        if (m_task_nodes[i] == task)
        {
            m_task_nodes[i] = m_task_nodes[m_task_counter - 1];
            m_task_counter--;
            PLACEMENT_DELETE(Task, task);
            break;
        }
    }
}

void TaskGraph::compile()
{
    uint32_t node_count = m_task_counter;
    uint32_t start_node_count = 0;

    Vector<uint32_t> start_nodes(node_count);
    for (uint32_t i = 0; i < node_count; ++i)
    {
        uint32_t dependency_count = m_task_nodes[i]->m_precede_nodes.size();
        if (dependency_count == 0)
        {
            start_nodes[start_node_count] = i;
            start_node_count++;
        }
        else
            m_task_nodes[i]->m_join_counter = dependency_count;
    }
    ASSERT(start_node_count > 0, "astd", "task graph must have at least one start node!");

    m_join_counter = 0;
    for (uint32_t i = 0; i < start_node_count; ++i)
    {
        swap(m_task_nodes[start_nodes[i]], m_task_nodes[m_join_counter]);
        m_join_counter++;
    }
}


AMAZING_NAMESPACE_END