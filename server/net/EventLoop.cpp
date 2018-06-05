//
// Created by jeff on 18-6-4.
//

#include <cassert>
#include "EventLoop.h"
#include "../base/Logging.h"

using namespace selfServer;
using namespace selfServer::net;

__thread EventLoop* t_loopInThisThread = 0;

EventLoop::EventLoop()
    : m_looping(false),
      m_threadId(static_cast<const pid_t>(syscall(SYS_gettid)))
{
    LOG_INFO << "EventLoop created in thread " << m_threadId;
    if (t_loopInThisThread)
    {
        LOG_FATAL << "Another EventLoop " << t_loopInThisThread
                                          << " exists in this thread " << m_threadId;
    } else
    {
        t_loopInThisThread = this;
    }
}

EventLoop::~EventLoop()
{
    assert(!m_looping);
    t_loopInThisThread = nullptr;
}

EventLoop *EventLoop::getEventLoopOfCurrentThread()
{
    return t_loopInThisThread;
}

void EventLoop::loop()
{
    assert(!m_looping);
    assertInLoopThread();
    m_looping = true;
    m_quit = false;
    LOG_INFO << "EventLoop start looping";

}

void EventLoop::abortNotInLoopThread()
{
    LOG_FATAL << "EventLoop::abortNotInLoopThread - EventLoop was created in threadID = "
                 << m_threadId << ", current threadID = " << syscall(SYS_gettid);
}

void EventLoop::quit()
{
    m_quit = true;

}

