//
// Created by AmazingBuff on 2025/6/10.
//

#ifndef THREAD_H
#define THREAD_H

#include "astd/base/define.h"
#include <thread>

AMAZING_NAMESPACE_BEGIN

class Thread
{
public:
    Thread() : m_running(false) {}
    virtual ~Thread()
    {
        stop();
    }

    void start()
    {
        if (!m_running)
        {
            m_thread = std::jthread([this](std::stop_token token)
            {
                this->run(std::move(token));
            });
            m_running = true;
        }
    }

    void stop()
    {
        m_thread.request_stop();
    }

    NODISCARD bool is_running() const
    {
        return m_running;
    }

    NODISCARD uint32_t id() const
    {
        std::jthread::id thread_id = m_thread.get_id();
        return *reinterpret_cast<uint32_t*>(&thread_id);
    }

protected:
    virtual void run(std::stop_token) = 0;

private:
    std::jthread m_thread;
    bool m_running;
};


AMAZING_NAMESPACE_END
#endif //THREAD_H
