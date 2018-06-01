//
// Created by jeff on 18-5-30.
//

#include <limits>
#include <cassert>
#include "LogStream.h"
using namespace selfServer;
using namespace selfServer::detail;



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

