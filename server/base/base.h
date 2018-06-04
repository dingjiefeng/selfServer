//
// Created by jeff on 18-5-29.
//

#ifndef WEBSERVER_BASE_H
#define WEBSERVER_BASE_H

#include <chrono>
#include <string>

class NonCopyable{
public:
    NonCopyable() = default;
    virtual ~NonCopyable() = default;

public:
    NonCopyable(const NonCopyable&) = delete;
    NonCopyable(const NonCopyable&&) = delete;
    NonCopyable&operator=(const NonCopyable&) = delete;
    NonCopyable&operator=(const NonCopyable&&) = delete;
};

class MyTime{
public:
    enum timeUnit{
        microsecond = 1,
        millisecond = 1000,
        second = 1000000,
        minute = 6000000
    };
    MyTime(): cur_t(std::chrono::steady_clock::now()), pre_t(std::chrono::steady_clock::now()) {}
    void mark()
    {
        pre_t = std::chrono::steady_clock::now();
    }

    double getDuration(timeUnit tu)
    {
        cur_t = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(cur_t - pre_t);
        return static_cast<double>(duration.count())/ static_cast<int>(tu);
    }

    std::string time2str(timeUnit& tu)
    {
        switch (tu)
        {
            case microsecond:
                return std::string("μs");
            case timeUnit::millisecond:
                return std::string("ms");
            case timeUnit::second:
                return std::string("s");
            case timeUnit::minute:
                return std::string("min");
            default:
                return std::string("null");
        }
    }

    std::string getDurationStr(timeUnit tu)
    {
        double du = getDuration(tu);
        std::string str(std::to_string(du) + time2str(tu));
        return str;
    }
private:

    //计算时间间隔, steady_clock更好
    std::chrono::steady_clock::time_point cur_t;
    std::chrono::steady_clock::time_point pre_t;

};
#endif //WEBSERVER_BASE_H
