//
// Created by jeff on 18-5-30.
//

#include <limits>
#include <cassert>
#include "LogStream.h"
using namespace selfServer;
using namespace selfServer::detail;

template <int SIZE>
void FixedBuffer<SIZE>::cookieStart()
{

}

template <int SIZE>
void FixedBuffer<SIZE>::cookieEnd()
{

}

void LogStream::staticCheck()
{
    static_assert(kMaxNumericSize - 10 > std::numeric_limits<double>::digits10);
    static_assert(kMaxNumericSize - 10 > std::numeric_limits<long double >::digits10);
    static_assert(kMaxNumericSize - 10 > std::numeric_limits<long>::digits10);
    static_assert(kMaxNumericSize - 10 > std::numeric_limits<long long>::digits10);
}

template <typename T>
void LogStream::formatInteger(T v)
{
    if (m_buffer.avail() >= kMaxNumericSize)
    {
        std::string str = std::to_string(v);
        *this << str;
    }
}

LogStream& LogStream::operator<<(short v)
{
    *this << static_cast<int>(v);
    return *this;
}


LogStream& LogStream::operator<<(unsigned short v)
{
    *this << static_cast<unsigned int>(v);
    return *this;
}


LogStream& LogStream::operator<<(int v)
{
    std::string str = std::to_string(v);
    *this << str;
    return *this;
}


LogStream& LogStream::operator<<(unsigned int v)
{
    std::string str = std::to_string(v);
    *this << str;
    return *this;
}

LogStream& LogStream::operator<<(long v)
{
    formatInteger(v);
    return *this;
}

LogStream& LogStream::operator<<(unsigned long v)
{
    formatInteger(v);
    return *this;
}

LogStream& LogStream::operator<<(unsigned long long v)
{
    formatInteger(v);
    return *this;
}

LogStream& LogStream::operator<<(const void* p)
{
//    uintptr_t v = reinterpret_cast<uintptr_t>(p);
//    if (buffer_.avail() >= kMaxNumericSize)
//    {
//        char* buf = buffer_.current();
//        buf[0] = '0';
//        buf[1] = 'x';
//        size_t len = convertHex(buf+2, v);
//        buffer_.add(len+2);
//    }
    return *this;
}

LogStream& LogStream::operator<<(double v)
{
    if (m_buffer.avail() >= kMaxNumericSize)
    {
        int len = snprintf(m_buffer.current(), kMaxNumericSize, "%.12g", v);
        m_buffer.add(static_cast<size_t>(len));
    }
    return *this;
}

template <typename T>
Fmt::Fmt(const char *fmt, T val)
{
    static_assert(std::is_arithmetic<T>::value != 0);

    m_len = snprintf(m_buf, sizeof(m_buf), fmt, val);
    assert(static_cast<size_t >(m_len) < sizeof(m_buf));
}

//explict
template Fmt::Fmt(const char* fmt, char);

template Fmt::Fmt(const char* fmt, short);
template Fmt::Fmt(const char* fmt, unsigned short);
template Fmt::Fmt(const char* fmt, int);
template Fmt::Fmt(const char* fmt, unsigned int);
template Fmt::Fmt(const char* fmt, long);
template Fmt::Fmt(const char* fmt, unsigned long);
template Fmt::Fmt(const char* fmt, long long);
template Fmt::Fmt(const char* fmt, unsigned long long);

template Fmt::Fmt(const char* fmt, float);
template Fmt::Fmt(const char* fmt, double);

