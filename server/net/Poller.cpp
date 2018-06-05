//
// Created by jeff on 18-6-5.
//

#include <cassert>
#include "Poller.h"
#include "Channel.h"
#include "../base/Logging.h"

using namespace selfServer;
using namespace selfServer::net;

Poller::Poller(EventLoop *loop)
    : m_ownerLoop(loop)
{

}

Poller::~Poller() = default;

void Poller::fillActiveChannels(int numEvents, Poller::ChannelList *activeChannels) const
{
    for (auto pfd = m_PollFds.cbegin();
            pfd != m_PollFds.cend() && numEvents > 0; ++pfd)
    {
        if (pfd->revents > 0)
        {
            --numEvents;
            auto ch = m_Channels.find(pfd->fd);
            assert(ch != m_Channels.end());
            Channel* p_channel = ch->second;
            assert(p_channel->fd() == pfd->fd);
            p_channel->set_revents(pfd->revents);
            activeChannels->emplace_back(p_channel);
        }
    }
}

steady_clock::time_point Poller::poll(int timeOutMs, Poller::ChannelList *activeChannels)
{
    int numEvents = ::poll(&*m_PollFds.begin(), m_PollFds.size(), timeOutMs);
    steady_clock::time_point now(steady_clock::now());
    if (numEvents > 0) {
        LOG_INFO << numEvents << " events happend";
        fillActiveChannels(numEvents, activeChannels);
    } else if (numEvents == 0) {
        LOG_INFO << "nothing happened";
    } else {
        LOG_INFO << "Poller::poll()";
    }
    return now;
}

void Poller::updateChannel(Channel *channel)
{
    assertInLoopThread();
    LOG_INFO << "fd= " << channel->fd() << "events = " << channel->events();
    if (channel->index() < 0) {
        assert(m_Channels.find(channel->fd()) == m_Channels.end());
        struct pollfd pfd;
        pfd.fd = channel->fd();
        pfd.events = static_cast<short>(channel->events());
        pfd.revents = 0;
        m_PollFds.emplace_back(pfd);
        int idx = static_cast<int>(m_PollFds.size() - 1);
        channel->set_index(idx);
        m_Channels[pfd.fd] = channel;
    } else {
        //update existing one
        assert(m_Channels.find(channel->fd()) != m_Channels.end());
        assert(m_Channels[channel->fd()] == channel);
        int idx = channel->index();
        assert(0<=idx && idx < static_cast<int>(m_PollFds.size()));
        struct pollfd& pfd = m_PollFds[idx];
        assert(pfd.fd == channel->fd() || pfd.fd == -1);
        pfd.events = static_cast<short>(channel->events());
        pfd.revents = 0;
        if (channel->isNoneEvent()) {
            pfd.fd = -1;
        }
    }
}
