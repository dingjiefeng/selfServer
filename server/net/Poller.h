//
// Created by jeff on 18-6-5.
//

#ifndef WEBSERVER_POLLER_H
#define WEBSERVER_POLLER_H


#include <vector>
#include <map>
#include "../base/base.h"
#include "EventLoop.h"
#include <chrono>
#include <sys/poll.h>
#include <sys/epoll.h>

//only use epoll

using namespace std::chrono;
namespace selfServer
{
    namespace net
    {
        class Channel;

        const int kNew = -1;
        const int kAdded = 1;
        const int kDeleted = 2;

        class Poller : public NonCopyable
        {
        public:
            typedef std::vector<Channel*> ChannelList;

            explicit Poller(EventLoop* loop);

            ~Poller() override ;
            virtual steady_clock::time_point poll(int timeOutMs, ChannelList* activeChannels) = 0;

            virtual void updateChannel(Channel* channel) = 0;

            virtual void removeChannel(Channel* channel) = 0;

            virtual bool hasChannel(Channel* channel) const;

            void assertInLoopThread() const { m_ownerLoop->assertInLoopThread();}

            static Poller* newDefaultPoller(EventLoop* loop);

        protected:
            typedef std::map<int, Channel*> ChannelMap;
            ChannelMap m_Channels;

        private:
            EventLoop* m_ownerLoop;

        };

        class EPollPoller : public Poller
        {
        public:
            explicit EPollPoller(EventLoop* loop);

            ~EPollPoller() override;

            steady_clock::time_point poll(int timeOutMs, ChannelList* activeChannels) override;

            void updateChannel(Channel* channel) override;

            void removeChannel(Channel* channel) override;

        private:
            static const int kInitEventListSize = 16;
            static std::string operationToString(int op);

            void fillActiveChannels(int numEvents,
                                    ChannelList* activeChannels) const;
            void update(int operation, Channel* channel);

            typedef std::vector<struct epoll_event> EventList;

            int m_epollFd;
            EventList m_events;
        };
    }
}
#endif //WEBSERVER_POLLER_H
