//
// Created by jeff on 18-5-29.
//

#ifndef WEBSERVER_BASE_H
#define WEBSERVER_BASE_H

class NonCopyable{
public:
    NonCopyable() = default;
    virtual ~NonCopyable() = default;

public:
    NonCopyable(const NonCopyable&) = delete;
    NonCopyable(const NonCopyable&&) = delete;
    NonCopyable&operator=(const NonCopyable&) = delete;
    NonCopyable&operator=(const NonCopyable&&) = delete;
};
#endif //WEBSERVER_BASE_H
