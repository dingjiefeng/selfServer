//
// Created by jeff on 18-5-29.
//

#include <cassert>
#include <memory>
#include <thread>
#include "LogFile.h"
#include "FileUtil.h"

using namespace selfServer;

LogFile::LogFile(const std::string &basename,
                 off_t rollsize,
                 bool threadSafe,
                 int flushInterval,
                 int checkEveryN)
: m_baseName(basename),
  m_rollSize(rollsize),
  m_flushInterval(flushInterval),
  m_checkEveryN(checkEveryN),
  m_count(0),
  mp_mutex(threadSafe ? new std::mutex : nullptr),
  m_startOfPeriod(0),
  m_lastRoll(0),
  m_lastFlush(0)
{
//    assert(basename.find('/') == std::string::npos);
    rollFile();
}

LogFile::~LogFile()
= default;

void LogFile::append(const char *logline, size_t len)
{
    if (mp_mutex)
    {
        std::unique_lock<std::mutex> lock(*mp_mutex);
        append_unlocked(logline, len);
    } else
    {
        append_unlocked(logline, len);
    }
}

void LogFile::flush()
{
    if (mp_mutex)
    {
        std::unique_lock<std::mutex> lock(*mp_mutex);
        mp_file->flush();
    } else
    {
        mp_file->flush();
    }
}

bool LogFile::rollFile()
{
    time_t now(0);
    std::string filename = getLogFileName(m_baseName, &now);
    time_t start = now/m_kRollPerSeconds*m_kRollPerSeconds;
    if (now > m_lastRoll)
    {
        m_lastRoll = now;
        m_lastFlush = now;
        m_startOfPeriod = start;
        mp_file.reset(new AppendFile(m_baseName));
        return true;
    }
    return false;
}

void LogFile::append_unlocked(const char *logline, size_t len)
{
    mp_file->append(logline, len);
    if (mp_file->writtenBytes() > m_rollSize)
    {
        rollFile();
    } else
    {
        ++m_count;
        if (m_count >= m_checkEveryN)
        {
            m_count = 0;
            //system_clock 表示系统时钟,因为系统时间可以被修改,所以有不稳定因素;steady_clock表示距离开机的相对时间只能用于计算时间间隔
            time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
            time_t thisPeriod = now/m_kRollPerSeconds*m_kRollPerSeconds;
            if (thisPeriod != m_startOfPeriod)
            {
                rollFile();
            } else if (now - m_lastFlush > m_flushInterval)
            {
                m_lastFlush = now;
                mp_file->flush();
            }
        }
    }
}

std::string LogFile::getLogFileName(const std::string &basename, time_t *now)
{
    std::string filename;
    filename.reserve(basename.size() + 64);
    filename = basename;

    char timebuf[32];
    struct tm tm{};
    *now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    gmtime_r(now, &tm);
    strftime(timebuf, sizeof timebuf, ".%Y%m%d-%H%M%S", &tm);
    filename += timebuf;

    char pidbuf[32];
    snprintf(pidbuf, sizeof pidbuf, ".%d", std::this_thread::get_id());
    filename += pidbuf;

    filename += ".log";

    return filename;
}
