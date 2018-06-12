//
// Created by jeff on 18-6-5.
//

#ifndef WEBSERVER_CHANNEL_H
#define WEBSERVER_CHANNEL_H


#include "../base/base.h"
#include <functional>
#include <memory>

using namespace std::chrono;

namespace selfServer
{
    namespace net
    {
        class EventLoop;
        /**
         * @brief
         */
        class Channel : public NonCopyable
        {
        public:
            typedef std::function<void()> EventCallback;
            //typedef std::function<void(steady_clock::time_point)> ReadEventCallback;

            Channel(EventLoop* loop, int fd);

            void handleEvent();

            void setReadCallback(EventCallback cb)
            { m_readCallback = std::move(cb);}
            void setWriteCallback(EventCallback cb)
            { m_writeCallback = std::move(cb);}
            void setErrorCallback(EventCallback cb)
            { m_errorCallback = std::move(cb);}

            int fd() const { return m_fd;}

            int events() const { return m_events;}

            void set_revents(int revt) { m_revents = revt;}

            bool isNoneEvent() const { return m_events == kNoneEvent;}

            void enableReading() { m_events |= kReadEvent; update();}
            void disableReading() { m_events &= ~kReadEvent; update(); }
            void enableWriting() { m_events |= kWriteEvent; update(); }
            void disableWriting() { m_events &= ~kWriteEvent; update(); }
            void disableAll() { m_events = kNoneEvent; update(); }
            bool isWriting() const { return static_cast<bool>(m_events & kWriteEvent); }
            bool isReading() const { return static_cast<bool>(m_events & kReadEvent); }


            //for poller
            int index() const { return m_index;}
            void set_index(const int idx) { m_index = idx;}

            EventLoop* ownerLoop() { return m_loop;}

            void remove();
        private:

            void update();

            static const int kNoneEvent;
            static const int kReadEvent;
            static const int kWriteEvent;

            EventLoop* m_loop;
            const int m_fd;
            int m_events;
            int m_revents;
            int m_index;

            EventCallback m_readCallback;
            EventCallback m_writeCallback;
            EventCallback m_errorCallback;
            EventCallback m_closeCallback;
        };
    }
}
#endif //WEBSERVER_CHANNEL_H
