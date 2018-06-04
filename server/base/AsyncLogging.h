//
// Created by jeff on 18-5-31.
//

#ifndef WEBSERVER_ASYNCLOGGING_H
#define WEBSERVER_ASYNCLOGGING_H

#include <vector>
#include <thread>
#include "base.h"
#include "LogStream.h"
#include "CountDownLatch.h"

namespace selfServer
{
    /**
     * @brief 主要负责多缓冲技术 协调前后端,更换缓冲区
     */
    class AsyncLogging : NonCopyable
    {
    public:
        explicit AsyncLogging(const std::string& basename,
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

        /**
         * @brief 启动工作线程
         */
        void start()
        {
            m_running = true;
            m_thread = std::thread(std::bind(&AsyncLogging::threadFunc, this));
            m_latch.wait();
        }

        /**
         * @brief 关闭工作线程
         */
        void stop()
        {
            m_running = false;
            m_cond.notify_one();
            m_thread.join();
        }

    private:

        void threadFunc();

        typedef selfServer::detail::FixedBuffer<detail::kLargeBuffer> Buffer;
        typedef std::unique_ptr<Buffer> BufferPtr;
        typedef std::vector<BufferPtr> BufferVector;

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
