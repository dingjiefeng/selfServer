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

namespace selfServer
{
    class any
    {
    public: // structors

        any() : content(0)
        {
        }

        template<typename ValueType>
        any(const ValueType & value)
                : content(new holder<ValueType>(value))
        {
        }

        any(const any & other)
                : content(other.content ? other.content->clone() : 0)
        {
        }

        ~any()
        {
            delete content;
        }

    public: // modifiers

        any & swap(any & rhs)
        {
            std::swap(content, rhs.content);
            return *this;
        }

        template<typename ValueType>
        any & operator=(const ValueType & rhs)
        {
            any(rhs).swap(*this);
            return *this;
        }

        any & operator=(any rhs)
        {
            rhs.swap(*this);
            return *this;
        }

    public: // queries

        bool empty() const
        {
            return !content;
        }

        const std::type_info & type() const
        {
            return content ? content->type() : typeid(void);
        }

        class placeholder
        {
        public: // structors

            virtual ~placeholder()
            {
            }

        public: // queries

            virtual const std::type_info & type() const = 0;

            virtual placeholder * clone() const = 0;

        };

        template<typename ValueType>
        class holder : public placeholder
        {
        public: // structors

            holder(const ValueType & value)
                    : held(value)
            {
            }

        public: // queries

            virtual const std::type_info & type() const
            {
                return typeid(ValueType);
            }

            virtual placeholder * clone() const
            {
                return new holder(held);
            }

        public: // representation

            ValueType held;

        private: // intentionally left unimplemented
            holder & operator=(const holder &);
        };

    public: // representation (public so any_cast can be non-friend)


        placeholder* content;

    };
}

#endif //WEBSERVER_BASE_H
