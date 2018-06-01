//
// Created by jeff on 18-5-31.
//

#include <cassert>
#include "AsyncLogging.h"
#include "LogFile.h"

using namespace selfServer;

/**
 * @brief 异步日志的构造函数
 * @param basename 日志的基本名
 * @param rollSize 与LogFile中类似
 * @param flushInterval 同上
 */
AsyncLogging::AsyncLogging(const std::string &basename,
                           off_t rollSize,
                           int flushInterval)
    : m_flushInterval(flushInterval),
      m_running(false),
      m_basename(basename),
      m_rollSize(rollSize),
      m_thread(),
      m_latch(1),
      m_mutex(),
      m_cond(),
      m_currentBuffer(new Buffer),
      m_nextBuffer(new Buffer),
      m_buffers()
{
    m_currentBuffer->bzero();
    m_nextBuffer->bzero();
    m_buffers.reserve(16);
}

/**
 * @brief 往缓冲区写入数据;寻找一个大小足够的缓冲区写入,如果当前缓冲区不够,就替换
 * @param logline 数据
 * @param len 数据长度
 */
void AsyncLogging::append(const char *logline, int len)
{
    std::unique_lock<std::mutex> lock(m_mutex);
    if (m_currentBuffer->avail() > len)
    {
        m_currentBuffer->append(logline, static_cast<size_t>(len));
    } else
    {
        m_buffers.emplace_back(m_currentBuffer.release());

        if (m_nextBuffer)
        {
            m_currentBuffer = std::move(m_nextBuffer);
        } else
        {
            m_currentBuffer.reset(new Buffer);
        }
        m_currentBuffer->append(logline, static_cast<size_t>(len));
        m_cond.notify_one();
    }
}

/**
 * @brief 线程函数,主要负责前后端缓冲区的调度
 */
void AsyncLogging::threadFunc()
{
    assert(m_running);
    m_latch.countDown();
    LogFile output(m_basename, m_rollSize, false);
    BufferPtr newBuffer1(new Buffer);
    BufferPtr newBuffer2(new Buffer);
    newBuffer1->bzero();
    newBuffer2->bzero();
    BufferVector buffersToWrite;
    buffersToWrite.reserve(16);
    while (m_running)
    {
        //二级缓冲区每次循环中都会被补充,并且重置数据指针
        //二级缓冲是未使用过的缓冲区
        assert(newBuffer1 && newBuffer1->length() == 0);
        assert(newBuffer2 && newBuffer2->length() == 0);
        //每次循环后 BTW会被清除
        assert(buffersToWrite.empty());
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            if (m_buffers.empty())//unusual usage!
            {
                m_cond.wait_for(lock, std::chrono::seconds(m_flushInterval));
            }
            m_buffers.emplace_back(m_currentBuffer.release());
            m_currentBuffer = std::move(newBuffer1);
            buffersToWrite.swap(m_buffers);
            if (!m_nextBuffer)
            {
                m_nextBuffer = std::move(newBuffer2);
            }
        }

        assert(!buffersToWrite.empty());

        if (buffersToWrite.size() > 25)
        {
//            char buf[256];
//            snprintf(buf, sizeof buf, "Dropped log messages at %s, %zd larger buffers\n",
//                     Timestamp::now().toFormattedString().c_str(),
//                     buffersToWrite.size()-2);
//            fputs(buf, stderr);
//            output.append(buf, static_cast<int>(strlen(buf)));
            buffersToWrite.erase(buffersToWrite.begin()+2, buffersToWrite.end());
        }

        for (auto &bufferToWrite : buffersToWrite)
        {
            output.append(bufferToWrite->data(),
                          static_cast<size_t>(bufferToWrite->length()));
        }

        if (buffersToWrite.size() > 2)
        {
            buffersToWrite.resize(2);
        }

        //如果二级缓存为空,则从buffersToWrite里面取出一个作为二级缓存
        if (!newBuffer1)
        {
            assert(!buffersToWrite.empty());
            newBuffer1 = std::move(buffersToWrite.back());
            buffersToWrite.pop_back();
            newBuffer1->reset(); // 重置缓冲区指针,mp_cur指向data的首地址
        }
        if (!newBuffer2)
        {
            assert(!buffersToWrite.empty());
            newBuffer2 = std::move(buffersToWrite.back());
            buffersToWrite.pop_back();
            newBuffer2.reset();
        }
        buffersToWrite.clear();
        output.flush();
    }
    output.flush();
}