//
// Created by jeff on 18-5-29.
//

#include <cassert>
#include <memory>
#include <thread>
#include "LogFile.h"
#include "FileUtil.h"

using namespace selfServer;



/**
 * @brief LogFile的构造函数
 * @param basename log文件的基本名称
 * @param rollsize 滚动的大小,当log文件大于该值时,建立新的log文件
 * @param threadSafe muduo log库的特点,可以根据是否线程安全的设计来考虑是否给操作加锁
 * @param flushInterval flush的间隔
 * @param checkEveryN
 */
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
    //建立第一个log文件
    rollFile();
}

LogFile::~LogFile()
= default;

/**
 * @brief 向文件中写入新的内容,实际先写入文件的缓冲区,达到一定大小后再flush,间接调用append_unlocked来实现
 * @param logline 字符串首地址
 * @param len 字符串长度
 */
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

/**
 * @brief flush操作 间接调用文件的flush
 */
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

/**
 * @brief 滚动log文件
 * @return
 */
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
        mp_file.reset(new AppendFile(filename));
        return true;
    }
    return false;
}

/**
 * @brief 向文件写内容的不加锁操作,这里牵扯到日志文件的滚动规则-->可以总结成尽可能的少rollFile()
 * 滚动规则为:
 *  1. 目前累计写入量>rollsize --> rollFile()
 *  2. 写入次数>m_checkEveryN --> 如果已经过了一天 -->roolFile()
 *                          --> 如果没有 --> 根据m_flushInterval判断是否清空缓冲区,更新相应值
 * @param logline 内容
 * @param len 长度
 */
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

/**
 * @brief 获取下一个log文件的名称 basename+年月日+时分秒+线程id.log
 * @param basename 基础名
 * @param now 时间
 * @return 返回最终文件名
 */
std::string LogFile::getLogFileName(const std::string &basename, time_t *now)
{
    std::string filename;
    filename.reserve(basename.size() + 64);
    filename = basename;

    char timebuf[32];
    struct tm tm{};
    *now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    gmtime_r(now, &tm);
    strftime(timebuf, sizeof timebuf, "-%Y%m%d-%H%M%S", &tm);
    filename += timebuf;

    char pidbuf[32];
    snprintf(pidbuf, sizeof pidbuf, "-%d", std::this_thread::get_id());
    filename += pidbuf;

    filename += ".log";

    return filename;
}
