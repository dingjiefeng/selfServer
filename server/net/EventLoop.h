//
// Created by jeff on 18-6-4.
//

#ifndef WEBSERVER_EVENTLOOP_H
#define WEBSERVER_EVENTLOOP_H

#include <functional>
#include <atomic>
#include <sys/syscall.h>
#include <zconf.h>
#include <mutex>
#include <vector>
#include <memory>
#include "Channel.h"
#include "../base/base.h"
#include "Poller.h"
#include <sys/socket.h>

using namespace std::chrono;

namespace selfServer
{
    namespace net
    {
        class Epoller;

        /**
         *@brief one loop one thread,
         * check if there is other thread in the same thread,
         * LOG_FATAL the meaasge if there is other loop
         *
         */
        class EventLoop : public NonCopyable
        {
        public:
            typedef std::function<void()> Functor;
            EventLoop();
            ~EventLoop() override;

            void loop();
            void quit();
            void runInLoop(Functor cb);
            void queueInLoop(Functor cb);
            size_t queueSize() const;
            void wakeup();
            //间接调用channel的接口实现
            void updateChannel(Channel* channel);
            void removeChannel(Channel* channel);
            bool hasChannel(Channel* channel);
            void assertInLoopThread()
            {
                if (!isInLoopThread())
                {
                    abortNotInLoopThread();
                }
            }
            bool isInLoopThread() const { return m_threadId == syscall(SYS_gettid);}
            static EventLoop* getEventLoopOfCurrentThread();
        private:
            void abortNotInLoopThread();
            void handleRead();
            void doPendingFunctors();

            typedef std::vector<Channel*> ChannelList;
            bool m_looping;
            bool m_quit;
            bool m_eventHandling;
            bool m_callingPendingFunctors;
            const pid_t m_threadId;
            std::shared_ptr<Epoller> m_epoller;
            int m_wakeupFd;
            std::shared_ptr<Channel> m_wakeupChannel;
            ChannelList m_activeChannels;
            Channel* m_currentActiveChannel;
            mutable std::mutex m_mutex;
            std::vector<Functor> m_pendingFunctors; // Guarded by m_mutex
        };
    }
}


#endif //WEBSERVER_EVENTLOOP_H
