//
// Created by jeff on 18-6-13.
//

#include "EventLoopThread.h"
#include "EventLoop.h"

using namespace selfServer;
using namespace selfServer::net;

EventLoopThread::EventLoopThread()
    : m_loop(nullptr),
      m_exiting(false),
      m_thread(),
      m_mutex(),
      m_cond()
{ }

EventLoopThread::~EventLoopThread()
{
    m_exiting = true;
    if (m_loop != nullptr)
    {
        m_loop->quit();
        m_thread.join();
    }
}

EventLoop *EventLoopThread::startLoop()
{
    m_thread = std::thread(std::bind(&EventLoopThread::threadFunc, this));
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        //阻塞直到m_loop不为空
        while (m_loop == nullptr)
            m_cond.wait(lock);
    }
    return m_loop;
}

void EventLoopThread::threadFunc()
{
    EventLoop loop;
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_loop = &loop;
        m_cond.notify_one();
    }
    loop.loop();
    m_loop = nullptr;
}
