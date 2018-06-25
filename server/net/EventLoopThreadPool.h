//
// Created by jeff on 18-6-25.
//

#ifndef WEBSERVER_EVENTLOOPTHREADPOOL_H
#define WEBSERVER_EVENTLOOPTHREADPOOL_H


#include <vector>
#include <bits/unique_ptr.h>
#include "../base/base.h"

namespace selfServer
{
    namespace net
    {
        class EventLoop;
        class EventLoopThread;

        class EventLoopThreadPool : public NonCopyable
        {
        public:
            explicit EventLoopThreadPool(EventLoop* baseLoop);
            ~EventLoopThreadPool() override = default;
            void setTheadNum(int numThreads) { m_numThreads = numThreads;}
            void start();

            EventLoop* getNextLoop();
            EventLoop* getLoopForHash(size_t hashCode);
            std::vector<EventLoop*> getAllLoops();

            bool started() { return m_started;}
        private:
            EventLoop* m_baseloop;
            bool m_started;
            int m_numThreads;
            int m_next;
            std::vector<std::unique_ptr<EventLoopThread>> m_threads;
            std::vector<EventLoop*> m_loops;

        };
    }
}
#endif //WEBSERVER_EVENTLOOPTHREADPOOL_H
