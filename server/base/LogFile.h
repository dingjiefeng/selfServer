//
// Created by jeff on 18-5-29.
//

#ifndef WEBSERVER_LOGFILE_H
#define WEBSERVER_LOGFILE_H

#include <string>
#include <bits/unique_ptr.h>
#include <mutex>
#include "base.h"
#include "FileUtil.h"

namespace selfServer{
    /**
     * @brief LogFile类,log文件的基础类,主要作用在于负责log文件的生成与滚动
     */
    class LogFile : public NonCopyable{
    public:

        explicit LogFile(const std::string& basename,
                         off_t rollsize,
                         bool threadSafe = true,
                         int flushInterval = 3,
                         int checkEveryN = 1024);

        ~LogFile() override;

        void append(const char* logline, size_t len);

        void flush();

        bool rollFile();

    private:
        void append_unlocked(const char* logline, size_t len);

        static std::string getLogFileName(const std::string& basename, time_t* now);

        const std::string m_baseName;
        const off_t m_rollSize;
        const int m_flushInterval;
        const int m_checkEveryN;

        int m_count;
        std::unique_ptr<std::mutex> mp_mutex;
        time_t m_startOfPeriod;
        time_t m_lastRoll;
        time_t m_lastFlush;
        std::unique_ptr<AppendFile> mp_file;

        const static int m_kRollPerSeconds = 60*60*24;
    };
}



#endif //WEBSERVER_LOGFILE_H
