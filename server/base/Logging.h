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

        static void setLogFileName(std::string &fileName)
        {
            g_logFileName = fileName;
        }

//        static std::string getLogFileName()
//        {
//            std::string str(g_logFileName);
//            return str;
//        }
        static std::string g_logFileName;

    private:

        class Impl
        {
        public:
            Impl(const char* file, int line);
            void formatTime();

            LogStream m_stream;
            int m_line;
            std::string m_basename;
        };
        Impl m_impl;
    };



#define LOG selfServer::Logger(__FILE__, __LINE__).stream()

}
#endif //WEBSERVER_LOGGING_H
