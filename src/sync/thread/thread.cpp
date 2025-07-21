//
// Created by AmazingBuff on 25-7-21.
//

#include <astd/sync/thread/thread.h>

AMAZING_NAMESPACE_BEGIN

Thread::Thread() : m_running(false) {}

Thread::~Thread()
{
    stop();
}

void Thread::start()
{
    if (!m_running)
    {
        m_thread = std::jthread([&](std::stop_token token)
        {
            this->run(std::move(token));
        });
        m_running = true;
    }
}

void Thread::stop()
{
    m_thread.request_stop();
}

bool Thread::is_running() const
{
    return m_running;
}

uint32_t Thread::id() const
{
    std::jthread::id thread_id = m_thread.get_id();
    return *reinterpret_cast<uint32_t*>(&thread_id);
}

AMAZING_NAMESPACE_END