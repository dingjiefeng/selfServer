//
// Created by jeff on 18-6-5.
//

#ifndef WEBSERVER_POLLER_H
#define WEBSERVER_POLLER_H


#include <vector>
#include <map>
#include "../base/base.h"
#include "EventLoop.h"
#include "Channel.h"
#include <chrono>
#include <sys/poll.h>
#include <sys/epoll.h>


using namespace std::chrono;
namespace selfServer
{
    namespace net
    {
        const int kNew = -1;
        const int kAdded = 1;
        const int kDeleted = 2;
        class Epoller : public NonCopyable
        {
        public:
            typedef std::vector<Channel*> ChannelList;

            explicit Epoller(EventLoop* loop);

            ~Epoller() override;

            steady_clock::time_point poll(int timeOutMs, ChannelList* activeChannels);

            void updateChannel(Channel* channel);

            void removeChannel(Channel* channel);
            virtual bool hasChannel(Channel* channel) const;
            void assertInLoopThread() const;

        private:
            typedef std::map<int, Channel*> ChannelMap;
            typedef std::vector<epoll_event> EventList;

            ChannelMap m_Channels;
            EventLoop* m_ownerLoop;

            static const int kInitEventListSize = 16;
            static std::string operationToString(int op);

            void fillActiveChannels(int numEvents,
                                    ChannelList* activeChannels) const;
            void update(int operation, Channel* channel);

            int m_epollFd;
            EventList m_events;
        };
    }
}
#endif //WEBSERVER_POLLER_H
