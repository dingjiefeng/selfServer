//
// Created by jeff on 18-5-29.
//

#include <cassert>
#include <cstring>
#include "FileUtil.h"

/**
 * @brief AppendFile 的构造函数;其中文件以"ae"的形式打开,a表示append,将结果附加在文件尾部,若文件不存在就创建文件;e表示O_CLOEXEC模式
 * @details O_CLOEXEC模式:
 *          1. 调用open函数O_CLOEXEC模式打开的文件描述符在执行exec调用新程序中关闭，且为原子操作;
 *          2. 调用open函数不使用O_CLOEXEC模式打开的文件描述符，然后调用fcntl 函数设置FD_CLOEXEC选项，
 *          效果和使用O_CLOEXEC选项open函数相同，但分别调用open、fcnt两个l函数，不是原子操作，多线程环境
 *          中存在竞态条件，故用open函数O_CLOEXEC选项代替之;
 *          3. 调用open函数O_CLOEXEC模式打开的文件描述符，或是使用fcntl设置FD_CLOEXEC选项，这二者得到（处理）的描述符在通过fork调用产生的子进程中均不被关闭;
 *          4. 调用dup族类函数得到的新文件描述符将清除O_CLOEXEC模式.
 * @param filename 打开的文件名
 */
AppendFile::AppendFile(std::string filename)
        : m_fp(fopen(filename.c_str(), "ae")) //选项说明见函数说明
{
    assert(m_fp);
    //设置文件缓冲
    ::setbuffer(m_fp, m_buf, sizeof m_buf);
}

AppendFile::~AppendFile() {
    ::fclose(m_fp);
}

void AppendFile::append(const char *logline, size_t len) {
    size_t n = this->write(logline, len);
    size_t remain = len - n;
    while (remain>0)
    {
        size_t x = write(logline+n, remain);
        if (x == 0)
        {
            int err_no = ferror(m_fp);
            if (err_no)
            {
                fprintf(stderr, "AppendFile::append() failed %s\n", strerror(err_no));
            }
            break;
        }
        n += x;
        remain = len - n;
    }
    m_writtenBytes += len;
}

void AppendFile::flush() {
    fflush(m_fp);
}

/**
 * @brief 封装了一层fwrite_unlocked(),该函数与fwrite的区别在于,写过程不加锁,速度更快
 * @param logline
 * @param len
 * @return
 */
size_t AppendFile::write(const char *logline, size_t len) {
    return ::fwrite_unlocked(logline, 1, len, m_fp);
}
