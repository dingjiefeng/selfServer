//
// Created by jeff on 18-5-24.
//

#ifndef WEBSERVER_COUNTDOWNLATCH_H
#define WEBSERVER_COUNTDOWNLATCH_H

#include <condition_variable>
#include <mutex>
#include "base.h"


namespace selfServer{
    /**
     * @brief 用于多线程的同步,例如某个线程等待其他多个线程的场景
     */
    class CountDownLatch: public NonCopyable {
    public:
        explicit CountDownLatch(int count);
        void wait();
        void countDown();

    private:
        mutable std::mutex m_mutex;
        std::condition_variable m_condition;
        int m_cnt;
    };
}

#endif //WEBSERVER_COUNTDOWNLATCH_H
