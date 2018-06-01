//
// Created by jeff on 18-5-30.
//

#include <sys/time.h>
#include "Logging.h"
#include "AsyncLogging.h"

using namespace selfServer;
static std::unique_ptr<AsyncLogging> gp_AsyncLogger;

std::string Logger::g_logFileName = "selfServer.log";
namespace selfServer
{

    void once_init()
    {
//        std::string str(Logger::getLogFileName());
        gp_AsyncLogger.reset(new AsyncLogging(Logger::g_logFileName));
        gp_AsyncLogger->start();
    }
    void output(const char* msg, int len)
    {
        gp_AsyncLogger->append(msg, len);
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
    {

    }

    Logger::~Logger()
    {
        m_impl.m_stream << " -- " << m_impl.m_basename << ':' << m_impl.m_line << '\n';
        const LogStream::Buffer& buf(Logger::stream().buffer());
        output(buf.data(), buf.length());
    }
}