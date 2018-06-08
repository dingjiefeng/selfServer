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
#include <sys/socket.h>

using namespace std::chrono;

namespace selfServer
{
    namespace net
    {
        class Poller;

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

            steady_clock::time_point pollReturnTime() const { return m_pollReturnTime;}

            int64_t iteration() const { return m_iteration;}

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

            bool eventHandling() const { return m_eventHandling;}

            void setContext(const selfServer::any& context)
            { m_context = context; }

            const selfServer::any& getContext() const
            { return m_context; }

            selfServer::any* getMutableContext()
            { return &m_context; }

            static EventLoop* getEventLoopOfCurrentThread();
        private:

            void abortNotInLoopThread();
            void handleRead();
            void doPendingFunctors();

            void printActiveChannels() const;

            typedef std::vector<Channel*> ChannelList;


            bool m_looping;
            std::atomic<bool> m_quit;
            bool m_eventHandling;
            bool m_callingPendingFunctors;
            int64_t m_iteration;

            const pid_t m_threadId;
            steady_clock::time_point m_pollReturnTime;

            std::unique_ptr<Poller> m_poller;

            int m_wakeupFd;

            std::unique_ptr<Channel> m_wakeupChannel;
            selfServer::any m_context;

            ChannelList m_activeChannels;
            Channel* m_currentActiveChannel;

            mutable std::mutex m_mutex;
            std::vector<Functor> m_pendingFunctors; // Guarded by m_mutex
        };
    }
}


#endif //WEBSERVER_EVENTLOOP_H
