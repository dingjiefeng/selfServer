//
// Created by jeff on 18-6-4.
//

#include <cassert>
#include "EventLoop.h"
#include "../base/Logging.h"
#include "Poller.h"
#include <sys/eventfd.h>

using namespace selfServer;
using namespace selfServer::net;

__thread EventLoop* t_loopInThisThread = nullptr;

const int kPollTimeMs = 1000000;
int createEventfd()
{
    int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (evtfd < 0)
    {
        LOG_FATAL << "Failed in eventfd";
        abort();
    }
    return evtfd;
}


EventLoop::EventLoop()
    : m_looping(false),
      m_quit(false),
      m_eventHandling(false),
      m_callingPendingFunctors(false),
      m_iteration(0),
      m_threadId(static_cast<const pid_t>(syscall(SYS_gettid))),
      m_poller(Poller::newDefaultPoller(this)),
      //todo timerqueue
      m_wakeupFd(createEventfd()),
      m_wakeupChannel(new Channel(this, m_wakeupFd)),
      m_currentActiveChannel(nullptr)

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
    m_wakeupChannel->setReadCallback(
            std::bind(&EventLoop::handleRead, this)
    );
    m_wakeupChannel->enableReading();

}

EventLoop::~EventLoop()
{
    LOG_INFO << "EventLoop " << this << " of thread " << m_threadId
             << " destructs in thread " << syscall(SYS_gettid);
    m_wakeupChannel->disableAll();
    m_wakeupChannel->remove();
    ::close(m_wakeupFd);
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
    LOG_INFO << "EventLoop start looping" << this << " start looping";

    while (!m_quit)
    {
        m_activeChannels.clear();
        m_pollReturnTime = m_poller->poll(kPollTimeMs)
    }
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

