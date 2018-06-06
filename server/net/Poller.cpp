//
// Created by jeff on 18-6-5.
//

#include <cassert>
#include "Poller.h"
#include "Channel.h"
#include "../base/Logging.h"

using namespace selfServer;
using namespace selfServer::net;

static_assert(EPOLLIN == POLLIN,        "epoll uses same flag values as poll");
static_assert(EPOLLPRI == POLLPRI,      "epoll uses same flag values as poll");
static_assert(EPOLLOUT == POLLOUT,      "epoll uses same flag values as poll");
static_assert(EPOLLRDHUP == POLLRDHUP,  "epoll uses same flag values as poll");
static_assert(EPOLLERR == POLLERR,      "epoll uses same flag values as poll");
static_assert(EPOLLHUP == POLLHUP,      "epoll uses same flag values as poll");

//poller
Poller::Poller(EventLoop *loop)
    : m_ownerLoop(loop)
{

}

Poller::~Poller() = default;

bool Poller::hasChannel(selfServer::net::Channel *channel) const
{
    assertInLoopThread();
    auto it = m_Channels.find(channel->fd());
    return it != m_Channels.end() && it->second == channel;
}

Poller *Poller::newDefaultPoller(EventLoop *loop)
{
    return new EPollPoller(loop);
}

//epoll
EPollPoller::EPollPoller(EventLoop *loop)
        : Poller(loop),
          m_events(kInitEventListSize),
          m_epollFd(::epoll_create1(EPOLL_CLOEXEC))
{
    if (m_epollFd < 0)
    {
        LOG_FATAL << "EPollPoller::EPollPoller";
    }
}

EPollPoller::~EPollPoller()
{
    ::close(m_epollFd);
}

time_point EPollPoller::poll(int timeOutMs, Poller::ChannelList *activeChannels)
{
    LOG_INFO << "fd total count " << m_Channels.size();
    int numEvents = ::epoll_wait(m_epollFd,
                                 &*m_events.begin(),
                                 static_cast<int>(m_events.size()),
                                 timeOutMs);
    int savedErrno = errno;
    steady_clock::time_point now(steady_clock::now());
    if (numEvents > 0)
    {
        LOG_INFO << numEvents << " events happened";
        fillActiveChannels(numEvents, activeChannels);
        if ( numEvents == m_events.size())
        {
            m_events.resize(m_events.size()*2);
        }
    } else if (numEvents == 0)
    {
        LOG_INFO << "nothing happened";
    } else
    {
        //error
        if (savedErrno != EINTR)
        {
            errno = savedErrno;
            LOG_INFO << "EPollPoller::Poll() Error";
        }
    }
    return now;
}

/**
 * EPOLL_CTL_ADD : 注册新的fd到epfd中
 * EPOLL_CTL_MOD : 修改已注册的fd的监听事件
 * EPOLL_CTL_DEL : 从epfd中删除一个fd
 * @param channel
 */
void EPollPoller::updateChannel(Channel *channel)
{
    Poller::assertInLoopThread();
    const int index = channel->index();
    LOG_INFO << "fd = " << channel->fd()
             << " events = " << channel->events()
             << " index = " << index;
    if (index == kNew || index == kDeleted)
    {
        int fd = channel->fd();
        if (index == kNew)
        {
            assert(m_Channels.find(fd) == m_Channels.end());
            m_Channels[fd] = channel;
        } else
        {
            assert(m_Channels.find(fd) != m_Channels.end());
            assert(m_Channels[fd] == channel);
        }
        channel->set_index(kAdded);
        update(EPOLL_CTL_ADD, channel);
    } else
    {
        int fd = channel->fd();
        assert(m_Channels.find(fd) != m_Channels.end());
        assert(m_Channels[fd] == channel);
        assert(index == kAdded);
        if (channel->isNoneEvent())
        {
            update(EPOLL_CTL_DEL, channel);
            channel->set_index(kDeleted);
        } else
        {
            update(EPOLL_CTL_MOD, channel);
        }
    }
}

void EPollPoller::removeChannel(Channel *channel)
{
    Poller::assertInLoopThread();
    int fd = channel->fd();
    LOG_INFO << "fd = " << fd;
    assert(m_Channels.find(fd) != m_Channels.end());
    assert(m_Channels[fd] == channel);
    assert(channel->isNoneEvent());
    int index = channel->index();
    assert(index == kAdded || index == kDeleted);
    size_t n = m_Channels.erase(fd);
    assert(n == 1);

    if (index == kAdded)
    {
        update(EPOLL_CTL_DEL, channel);
    }
    channel->set_index(kNew);
}

std::string EPollPoller::operationToString(int op)
{
    switch (op)
    {
        case EPOLL_CTL_ADD:
            return "ADD";
        case EPOLL_CTL_DEL:
            return "DEL";
        case EPOLL_CTL_MOD:
            return "MOD";
        default:
            LOG_FATAL << "Unknown Operation";
            assert(false);
            return "Unknown Operation";
    }
}

void EPollPoller::fillActiveChannels(int numEvents,
                                     Poller::ChannelList *activeChannels) const
{
    assert(numEvents <= m_events.size());
    for (int i = 0; i < numEvents; ++i)
    {
        auto channel = static_cast<Channel *>(m_events[i].data.ptr);
        channel->set_revents(m_events[i].events);
        activeChannels->emplace_back(channel);
    }
}

void EPollPoller::update(int operation, Channel *channel)
{
    struct epoll_event event;
    bzero(&event, sizeof event);
    event.events = channel->events();
    event.data.ptr = channel;
    int fd = channel->fd();
    LOG_INFO << "epoll_ctl op = " << operationToString(operation)
             << " fd = " << fd
                         << " event = { " << channel->

}
