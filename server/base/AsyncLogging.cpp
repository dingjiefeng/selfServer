//
// Created by jeff on 18-5-31.
//

#include "AsyncLogging.h"

using namespace selfServer;

AsyncLogging::AsyncLogging(const std::string &basename,
                           off_t rollSize,
                           int flushInterval)
    : m_flushInterval(flushInterval),
      m_running(false),
      m_basename(basename),
      m_rollSize(rollSize),
      m_thread(),
      m_latch(1),
      m_mutex(),
      m_cond(),
      m_currentBuffer(new Buffer),
      m_nextBuffer(new Buffer),
      m_buffers()
{
    m_currentBuffer->bzero();
    m_nextBuffer->bzero();
    m_buffers.reserve(16);
}