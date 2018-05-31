//
// Created by jeff on 18-5-31.
//

#ifndef WEBSERVER_ASYNCLOGGING_H
#define WEBSERVER_ASYNCLOGGING_H

#include <vector>
#include <bits/shared_ptr.h>
#include <thread>
#include "base.h"
#include "LogStream.h"
#include "CountDownLatch.h"

namespace selfServer
{
    class AsyncLogging : NonCopyable
    {
    public:
        AsyncLogging(const std::string& basename,
                     off_t rollSize,
                     int flushInterval = 3);

        ~AsyncLogging() override
        {
            if (m_running)
            {
                stop();
            }
        }

        void append(const char* logline, int len);

        void start()
        {
            m_running = true;
            m_latch.wait();
        }

        void stop()
        {
            m_running = false;
            m_cond.notify_all();
            m_thread.join();
        }

    private:

        void threadFunc();

        typedef selfServer::detail::FixedBuffer<detail::kLargeBuffer> Buffer;
        typedef std::vector<std::shared_ptr<Buffer>> BufferVector;
        typedef std::shared_ptr<Buffer> BufferPtr;

        const int m_flushInterval;
        bool m_running;
        std::string m_basename;
        off_t m_rollSize;
        std::thread m_thread;
        selfServer::CountDownLatch m_latch;
        std::mutex m_mutex;
        std::condition_variable m_cond;
        BufferPtr m_currentBuffer;
        BufferPtr m_nextBuffer;
        BufferVector m_buffers;

    };
}
#endif //WEBSERVER_ASYNCLOGGING_H
