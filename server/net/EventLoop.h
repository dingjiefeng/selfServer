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
#include "../base/base.h"

//todo : wait for updating in the future when adding other files

namespace selfServer
{
    namespace net
    {
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

            bool m_looping;
            std::atomic<bool> m_quit;
            bool m_eventHandling;
            bool m_callingPendingFunctors;
            int64_t m_iteration;

            const pid_t m_threadId;
            std::chrono::system_clock::time_point m_pollReturnTime;

            mutable std::mutex m_mutex;
            std::vector<Functor> m_pendingFunctors; // Guarded by m_mutex
        };
    }
}


#endif //WEBSERVER_EVENTLOOP_H
