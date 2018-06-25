//
// Created by jeff on 18-6-25.
//

#include "EventLoopThreadPool.h"
#include "EventLoop.h"
#include "EventLoopThread.h"

using namespace selfServer;
using namespace selfServer::net;

EventLoopThreadPool::EventLoopThreadPool(EventLoop *baseLoop)
    : m_baseloop(baseLoop),
      m_started(false),
      m_numThreads(0),
      m_next(0)
{
}

void EventLoopThreadPool::start()
{
    m_baseloop->assertInLoopThread();
    m_started = true;

    for (int i = 0; i < m_numThreads; ++i)
    {
        auto * t = new EventLoopThread;
        m_threads.emplace_back(std::unique_ptr<EventLoopThread>(t));
        m_loops.emplace_back(t->startLoop());
    }
}

EventLoop *EventLoopThreadPool::getNextLoop()
{
    m_baseloop->assertInLoopThread();
    EventLoop* loop = m_baseloop;

    if (!m_loops.empty())
    {
        loop = m_loops[m_next];
        ++m_next;
        if (m_next >= m_loops.size())
        {
            m_next = 0;
        }
    }
    return loop;
}

EventLoop *EventLoopThreadPool::getLoopForHash(size_t hashCode)
{
    m_baseloop->assertInLoopThread();
    EventLoop* loop = m_baseloop;

    if (!m_loops.empty())
    {
        loop = m_loops[hashCode % m_loops.size()];
    }
    return loop;
}

std::vector<EventLoop *> EventLoopThreadPool::getAllLoops()
{
    m_baseloop->assertInLoopThread();
    if (m_loops.empty())
    {
        return std::vector<EventLoop*>(1, m_baseloop);
    } else
    {
        return m_loops;
    }
}




