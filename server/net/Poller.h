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


using namespace std::chrono;
namespace selfServer
{
    namespace net
    {
        class Channel;

        class Poller : public NonCopyable
        {
        public:
            typedef std::vector<Channel*> ChannelList;

            explicit Poller(EventLoop* loop);
            ~Poller() override ;
            steady_clock::time_point poll(int timeOutMs, ChannelList* activeChannels);

            void updateChannel(Channel* channel);

            void assertInLoopThread() { m_ownerLoop->assertInLoopThread();}


        private:
            void fillActiveChannels(int numEvents,
                                    ChannelList* activeChannels) const;
            typedef std::map<int, Channel*> ChannelMap;
            typedef std::vector<struct pollfd> PollFdList;

            EventLoop* m_ownerLoop;
            ChannelMap m_Channels;
            PollFdList m_PollFds;

        };
    }
}
#endif //WEBSERVER_POLLER_H
