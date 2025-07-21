//
// Created by AmazingBuff on 2025/6/10.
//

#ifndef THREAD_H
#define THREAD_H

#include <thread>
#include "astd/base/macro.h"

AMAZING_NAMESPACE_BEGIN

class Thread
{
public:
    Thread();
    virtual ~Thread();

    void start();
    void stop();

    NODISCARD bool is_running() const;
    NODISCARD uint32_t id() const;

    Thread(Thread&&) = default;
    Thread& operator=(Thread&&) = default;
protected:
    virtual void run(std::stop_token) = 0;

private:
    std::jthread m_thread;
    bool m_running;
};


AMAZING_NAMESPACE_END
#endif //THREAD_H
