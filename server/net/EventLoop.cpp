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

const int kPollTimeMs = 10000;
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

/**
 * @brief 构造函数 初始化各个变量
 * 同时检测有没有运行在多个线程中(按道理应该不会多个线程构造同一个对象)
 */
EventLoop::EventLoop()
    : m_looping(false),
      m_quit(false),
      m_eventHandling(false),
      m_callingPendingFunctors(false),
      m_iteration(0),
      m_threadId(static_cast<const pid_t>(syscall(SYS_gettid))),
      m_poller(Poller::newDefaultPoller(this)),
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

    //bind(EventLoop::handleRead, this) 显示和void(*)()不匹配,很奇怪
    //就用lambda做了一层包装
    m_wakeupChannel->setReadCallback(
            std::bind([](EventLoop* &ptr){ ptr->handleRead();},
                      this)
    );
    m_wakeupChannel->enableReading();

}

/**
 * 析构函数
 *
 */
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

/**
 * @brief EventLoop的核心部分
 * 每次循环前检测m_quit确定是否继续循环
 * 通过参数-返回值的方式调用poll返回活跃的channel
 * 依次处理
 */
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
        m_pollReturnTime = m_poller->poll(kPollTimeMs, &m_activeChannels);
        ++m_iteration;

        m_eventHandling = true;
        for (Channel* channel : m_activeChannels)
        {
            m_currentActiveChannel = channel;
            m_currentActiveChannel->handleEvent();
        }
        m_currentActiveChannel = nullptr;
        m_eventHandling = false;
        doPendingFunctors();
    }
    LOG_INFO << "EventLoop " << this << " stop looping";
    m_looping = false;
}

void EventLoop::abortNotInLoopThread()
{
    LOG_FATAL << "EventLoop::abortNotInLoopThread - EventLoop was created in threadID = "
                 << m_threadId << ", current threadID = " << syscall(SYS_gettid);
}

void EventLoop::quit()
{
    m_quit = true;
    if (!isInLoopThread())
    {
        wakeup();
    }
}

void EventLoop::runInLoop(EventLoop::Functor cb)
{
    if (isInLoopThread())
    {
        cb();
    } else
    {
        queueInLoop(std::move(cb));
    }
}

void EventLoop::queueInLoop(EventLoop::Functor cb)
{
    {
        std::lock_guard<std::mutex> lockGuard(m_mutex);
        m_pendingFunctors.push_back(std::move(cb));
    }
    if (!isInLoopThread() || m_callingPendingFunctors)
    {
        wakeup();
    }

}

size_t EventLoop::queueSize() const
{
    std::lock_guard<std::mutex> lockGuard(m_mutex);
    return m_pendingFunctors.size();
}

void EventLoop::wakeup()
{
    uint64_t one = 1;
    ssize_t n = ::write(m_wakeupFd, &one, sizeof one);
    if (n != sizeof one)
    {
        LOG_FATAL << "EventLoop::handleRead() reads " << n << " bytes instead of 8";
    }
}

void EventLoop::updateChannel(Channel *channel)
{
    assert(channel->ownerLoop() == this);
    assertInLoopThread();
    m_poller->updateChannel(channel);
}

void EventLoop::removeChannel(Channel *channel)
{
    assert(channel->ownerLoop() == this);
    assertInLoopThread();
    if (m_eventHandling)
    {
        assert(m_currentActiveChannel == channel ||
               std::find(m_activeChannels.begin(), m_activeChannels.end(), channel) == m_activeChannels.end());
    }
    m_poller->removeChannel(channel);
}

bool EventLoop::hasChannel(Channel *channel)
{
    assert(channel->ownerLoop() == this);
    assertInLoopThread();
    return m_poller->hasChannel(channel);
}

void EventLoop::handleRead()
{
    uint64_t one = 1;
    ssize_t n = read(m_wakeupFd, &one, sizeof one);
    if (n != sizeof one)
    {
        LOG_FATAL << "EventLoop::handleRead() reads " << n << " bytes instead of 8";
    }
}

void EventLoop::doPendingFunctors()
{
    std::vector<Functor> functors;
    m_callingPendingFunctors = true;

    {
        std::lock_guard<std::mutex> lock(m_mutex);
        functors.swap(m_pendingFunctors);
    }

    for (auto &functor : functors) {
        functor();
    }
    m_callingPendingFunctors = false;
}

void EventLoop::printActiveChannels() const
{
    for (const Channel* channel : m_activeChannels)
    {
        LOG_INFO << "{" << channel->reventsToString() << "} ";
    }
}






