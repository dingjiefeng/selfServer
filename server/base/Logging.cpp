//
// Created by jeff on 18-5-30.
//

#include <sys/time.h>
#include "Logging.h"


namespace selfServer
{

    void output(const char* msg, int len)
    {

    }

    Logger::Impl::Impl(const char *file, int line)
        : m_stream(),
          m_basename(file),
          m_line(line)
    {}

    void Logger::Impl::formatTime()
    {
        struct timeval tv;
        time_t time;
        char str_t[26] = {0};
        gettimeofday(&tv, nullptr);
        time = tv.tv_sec;
        struct tm* p_time = localtime(&time);
        strftime(str_t, 26, "%Y-%m-%d %H:%M:%S\n", p_time);
    }
    Logger::Logger(const char *file, int line)
        : m_impl(file, line)
    {}

    Logger::~Logger()
    {
        m_impl.finish();
        const LogStream::Buffer& buf(stream().buffer());
        output(buf.data(), buf.length());
    }
}