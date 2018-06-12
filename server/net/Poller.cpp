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


/**
 * @brief 构造函数 初始化宿主eventloop
 * epoller 和 eventloop ,channel之间的关系是
 *  eventloop只存在于一个线程中, loop拥有一个poller负责IO multiplexing
 *  poller与loop拥有相同的生命期,且只被一个loop拥有,所以无需考虑多线程race,操作无需加锁
 *  poller不拥有channel,poller间接的调用channel,故在poller析构之前需要调用removeChannel来unregister
 *
 * @param loop
 */
Epoller::Epoller(EventLoop *loop)
        : m_ownerLoop(loop),
          m_events(kInitEventListSize),
          m_epollFd(::epoll_create1(EPOLL_CLOEXEC))
{
    if (m_epollFd < 0)
    {
        LOG_FATAL << "Epoller::Epoller";
    }
}

/**
 * @brief epoller并不拥有channel,所以析构函数中不需要delete channel*
 */
Epoller::~Epoller()
{
    ::close(m_epollFd);
}

bool Epoller::hasChannel(selfServer::net::Channel *channel) const
{
    assertInLoopThread();
    auto it = m_Channels.find(channel->fd());
    return it != m_Channels.end() && it->second == channel;
}
/**
 * @brief
 * int epoll_wait(int epfd, struct epoll_event* events, int maxEvents, int timeout)
 * epfd : epoll_fd
 * events: 从内核得到时间的集合
 * maxEvents : 集合的大小
 * timeout : 超时时间
 * 返回值 :==0表示超时
 *        >0 表示事件个数
 * @param timeOutMs
 * @param activeChannels
 * @return
 */
steady_clock::time_point Epoller::poll(int timeOutMs, ChannelList *activeChannels)
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
        //如果size达到既定大小 主动进行扩容
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
            LOG_INFO << "Epoller::Poll() Error";
        }
    }
    return now;
}

/**
 * EPOLL_CTL_ADD : 注册新的fd到epfd中
 * EPOLL_CTL_MOD : 修改已注册的fd的监听事件
 * EPOLL_CTL_DEL : 从epfd中删除一个fd
 * @brief
 * @param channel
 */
void Epoller::updateChannel(Channel *channel)
{
    assertInLoopThread();
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

void Epoller::removeChannel(Channel *channel)
{
    assertInLoopThread();
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

std::string Epoller::operationToString(int op)
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

void Epoller::fillActiveChannels(int numEvents,
                                     ChannelList *activeChannels) const
{
    assert(numEvents <= m_events.size());
    for (int i = 0; i < numEvents; ++i)
    {
        auto channel = static_cast<Channel *>(m_events[i].data.ptr);
        channel->set_revents(m_events[i].events);
        activeChannels->emplace_back(channel);
    }
}

void Epoller::update(int operation, Channel *channel)
{
    struct epoll_event event{};
    bzero(&event, sizeof event);
    event.events = static_cast<uint32_t>(channel->events());
    event.data.ptr = channel;
    int fd = channel->fd();
    LOG_INFO << "epoll_ctl op = " << operationToString(operation)
             << " fd = " << fd;
    if (::epoll_ctl(m_epollFd, operation, fd, &event) < 0)
    {
        LOG_FATAL << "epoll_ctl op =" << operationToString(operation) << " fd =" << fd;
    }

}
