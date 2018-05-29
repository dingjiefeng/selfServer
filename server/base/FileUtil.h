//
// Created by jeff on 18-5-29.
//

#ifndef WEBSERVER_FILEUTIL_H
#define WEBSERVER_FILEUTIL_H

#include <string>
#include "base.h"

/**
 * 向文件中添加;not thread-safety
 */
class AppendFile : public NonCopyable{
public:
    explicit AppendFile(std::string filename);
    ~AppendFile() override;

    //append 向文件写
    void append(const char *logline, size_t len);
    void flush();

    const off_t writtenBytes() const { return m_writtenBytes;}
private:
    size_t write(const char *logline, size_t len);
    FILE* m_fp;
    char m_buf[64*1024];
    off_t m_writtenBytes;
};




#endif //WEBSERVER_FILEUTIL_H
