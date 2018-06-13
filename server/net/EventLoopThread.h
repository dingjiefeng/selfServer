//
// Created by jeff on 18-6-13.
//

#ifndef WEBSERVER_EVENTLOOPTHREAD_H
#define WEBSERVER_EVENTLOOPTHREAD_H

#include <functional>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "../base/base.h"

namespace selfServer
{
    namespace net
    {
        class EventLoop;

        class EventLoopThread : public NonCopyable
        {
        public:

            EventLoopThread();
            ~EventLoopThread() override ;
            EventLoop* startLoop();

        private:
            void threadFunc();

            EventLoop* m_loop;
            bool m_exiting;
            std::thread m_thread;
            std::mutex m_mutex;
            std::condition_variable m_cond;
        };

    }
}
#endif //WEBSERVER_EVENTLOOPTHREAD_H
