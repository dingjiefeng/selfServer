//
// Created by jeff on 18-5-23.
//

#ifndef WEBSERVER_THREADPOOL_H
#define WEBSERVER_THREADPOOL_H

#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <functional>
#include <future>
#include <queue>
#include <type_traits>
#include <utility>
#include <vector>
#include <iostream>
#include "base.h"

// C++11 版的 线程池
/**
 * @reference https://github.com/lizhenghn123/zl_threadpool
 * @brief C++ 11版本的线程池
 */
namespace selfServer
{
    /**
     *@brief 线程管理类
     */
class ThreadsGuard : public NonCopyable
    {
    public:

        explicit ThreadsGuard(std::vector<std::thread>& v)
                : threads_(v)
        {

        }

        /**
         * @brief 析构函数,等待所有线程运行结束释放资源
         */
        ~ThreadsGuard() override {
            for (size_t i = 0; i != threads_.size(); ++i)
            {
                if (threads_[i].joinable())
                {
                    threads_[i].join();
                }
            }
        }

    private:
        //将拷贝构造函数等申明为delete,避免自动生成
//        ThreadsGuard(ThreadsGuard&& tg) = delete;
//        ThreadsGuard& operator = (ThreadsGuard&& tg) = delete;

//        ThreadsGuard(const ThreadsGuard&) = delete;
//        ThreadsGuard& operator = (const ThreadsGuard&) = delete;
    private:
        std::vector<std::thread>& threads_; //线程vector的引用
    };


    class ThreadPool : public NonCopyable
    {
    public:
        typedef std::function<void()> task_type;

    public:
        explicit ThreadPool(int n = 0);

        /**
         * @brief 线程管理类自动释放资源,无需显示的释放线程
         */
        ~ThreadPool () override
        {
            stop();
            cond_.notify_all();
        }

        void stop()
        {
            stop_.store(true, std::memory_order_release);
        }

        /**
         * @brief 添加任务,可以取得返回值
         * @tparam Function 任务函数的类型
         * @tparam Args 任务参数的类型,这里使用了递归的模板参数
         * @return
         */
        template<typename Function, typename... Args>
        std::future<typename std::result_of<Function(Args...)>::type> add(Function&&, Args&&...);

    public:
//        ThreadPool(ThreadPool&&) = delete;
//        ThreadPool& operator = (ThreadPool&&) = delete;
//        ThreadPool(const ThreadPool&) = delete;
//        ThreadPool& operator = (const ThreadPool&) = delete;

    private:
        std::atomic<bool> stop_;
        std::mutex mtx_;
        std::condition_variable cond_;

        std::queue<task_type> tasks_;
        std::vector<std::thread> threads_;
        selfServer::ThreadsGuard tg_;
    };


    inline ThreadPool::ThreadPool(int n)
            : stop_(false)
            , tg_(threads_)
    {
        int nthreads = n;
        if (nthreads <= 0)
        {
            nthreads = std::thread::hardware_concurrency();
            nthreads = (nthreads == 0 ? 2 : nthreads);
        }

        for (int i = 0; i < nthreads; ++i)
        {
            threads_.emplace_back(std::thread([this]()->void {
                while (!stop_.load(std::memory_order_acquire))
                {
                    task_type task;
                    {
                        std::unique_lock<std::mutex> ulk(this->mtx_);
                        this->cond_.wait(ulk, [this]{ return stop_.load(std::memory_order_acquire) || !this->tasks_.empty(); });
                        if (stop_.load(std::memory_order_acquire))
                            return;
                        task = std::move(this->tasks_.front());
                        this->tasks_.pop();
                    }
                    task();
                }
            }));
        }
    }

    template<class Function, class... Args>
    std::future<typename std::result_of<Function(Args...)>::type>
    ThreadPool::add(Function&& fcn, Args&&... args)
    {
        typedef typename std::result_of<Function(Args...)>::type return_type;
        typedef std::packaged_task<return_type()> task;

        auto t = std::make_shared<task>(std::bind(std::forward<Function>(fcn), std::forward<Args>(args)...));
        auto ret = t->get_future();
        {
            std::lock_guard<std::mutex> lg(mtx_);
            if (stop_.load(std::memory_order_acquire))
                throw std::runtime_error("thread pool has stopped");
            tasks_.emplace([t]{(*t)(); });
        }
        cond_.notify_one();
        return ret;
    }
}
#endif //WEBSERVER_THREADPOOL_H
