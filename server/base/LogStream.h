//
// Created by jeff on 18-5-30.
//

#ifndef WEBSERVER_LOGSTREAM_H
#define WEBSERVER_LOGSTREAM_H

#include <cstdio>
#include <memory.h>
#include "base.h"
#include <string>
#include <functional>

namespace selfServer
{
    namespace detail
    {
        const int kSmallBuffer = 4000;
        const int kLargeBuffer = 4000*1000;

        template <int SIZE>
        class FixedBuffer : NonCopyable
        {
        public:
            FixedBuffer()
                    : m_data(),
                      mp_cur(m_data)
            {
                setCookie(cookieStart);
            }
            ~FixedBuffer() override
            {
                setCookie(cookieEnd);
            }

            void append(const char* buf, size_t len)
            {
                if (avail() > len)
                {
                    memcpy(mp_cur, buf, len);
                    mp_cur += len;
                }
            }

            const char* data() const { return m_data;}
            const int length() const { return static_cast<const int>(mp_cur - m_data);}

            char* current() { return mp_cur;}
            const int avail() const { return static_cast<int>(end() - mp_cur);}
            void add(size_t len) { mp_cur += len;}

            void reset() { mp_cur = m_data;}
            void bzero() { ::bzero(m_data, sizeof(m_data));}

            void setCookie(void (*cookie)()) { mpf_cookie = cookie;}

            std::string toString() const { return std::string(m_data, static_cast<unsigned long>(length()));}



        private:
            const char* end() const { return m_data+ sizeof(m_data);}

            static void cookieStart();
            static void cookieEnd();


            std::function<void()> mpf_cookie;
//            void (*mpf_cookie)();
            char m_data[SIZE];
            char* mp_cur;
        };
    }

    class LogStream : NonCopyable
    {
        typedef LogStream self;
    public:
        typedef detail::FixedBuffer<detail::kSmallBuffer> Buffer;
        self& operator<<(bool v)
        {
            m_buffer.append(v ? "1" : "0", 1);
            return *this;
        }

        self&operator<<(short);
        self&operator<<(unsigned short);
        self&operator<<(int);
        self&operator<<(unsigned int);
        self&operator<<(long);
        self&operator<<(unsigned long);
        self&operator<<(unsigned long long);

        self&operator<<(const void*);

        self&operator<<(double);

        self&operator<<(char v)
        {
            m_buffer.append(&v, 1);
            return *this;
        }


        self&operator<<(const char* str)
        {
            if (str)
            {
                m_buffer.append(str, strlen(str));
            } else
            {
                m_buffer.append("(null)", 6);
            }
            return *this;
        }

        self&operator<<(const std::string& v)
        {
            m_buffer.append(v.c_str(), v.size());
            return *this;
        }

        self&operator<<(const Buffer& v)
        {
            m_buffer.append(v.data(), static_cast<size_t>(v.length()));
            return *this;
        }

        void append(const char* data, int len)
        {
            m_buffer.append(data, static_cast<size_t>(len));
        }

        const Buffer& buffer() const { return m_buffer;}

        void resetBuffer() { m_buffer.reset();}

    private:
        void staticCheck();

        template <typename T>
        void formatInteger(T);

        Buffer m_buffer;

        static const int kMaxNumericSize = 32;
    };

    class Fmt
    {
    public:
        template <typename T>
        Fmt(const char* fmt, T val);

        const char* data() const { return m_buf;}
        const int length() const { return m_len;}
    private:
        char m_buf[32];
        int m_len;
    };

    inline LogStream& operator<<(LogStream& s, const Fmt& fmt)
    {
        s.append(fmt.data(), fmt.length());
        return s;
    }
}

#endif //WEBSERVER_LOGSTREAM_H
