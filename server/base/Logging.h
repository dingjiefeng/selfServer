//
// Created by jeff on 18-5-30.
//

#ifndef WEBSERVER_LOGGING_H
#define WEBSERVER_LOGGING_H


#include <cstring>
#include "LogStream.h"

namespace selfServer
{
    class Logger
    {
    public:

        Logger(const char* file, int line);
        ~Logger();

        LogStream& stream() { return m_impl.m_stream;}

        typedef std::function<void(const char*, int)> OutputFunc;
        typedef std::function<void()> FlushFunc;

        static void setOutput(OutputFunc);
        static void setFlush(FlushFunc);

    private:

        class Impl
        {
        public:
            Impl(const char* file, int line);
            void formatTime();
            void finish();

            LogStream m_stream;
            int m_line;
            std::string m_basename;
        };
        Impl m_impl;
    };



#define LOG selfServer::Logger(__FILE__, __LINE__).stream()


}
#endif //WEBSERVER_LOGGING_H
