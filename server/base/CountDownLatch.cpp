//
// Created by jeff on 18-5-24.
//

#include "CountDownLatch.h"

using namespace selfServer;

CountDownLatch::CountDownLatch(int count):
m_mutex(),
m_condition(),
m_cnt(count)
{}

/**
 *@brief 等待函数,等待m_cnt减小到0
 */
void CountDownLatch::wait()
{
    std::unique_lock<std::mutex> lock(m_mutex);
    while (m_cnt > 0)
        m_condition.wait(lock);
}

/**
 *@brief 减小m_cnt,当等于0的时候就唤醒等待线程
 */
void CountDownLatch::countDown()
{
    std::unique_lock<std::mutex> lock(m_mutex);
    --m_cnt;
    if (m_cnt == 0)
        m_condition.notify_all();
}



