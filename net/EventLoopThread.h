#ifndef NET_EVENTLOOPTHREAD_H
#define NET_EVENTLOOPTHREAD_H
#include <mutex>
#include <condition_variable>
#include "Callbacks.h"
// #include "EventLoop.h"
#include "../base/Thread.h"

class EventLoopThread {
  public:
    EventLoopThread(const std::string& name = std::string());
    // EventLoopThread(const ThreadInitCallback& callback = ThreadInitCallback(), const std::string& name = std::string());
    ~EventLoopThread();
    EventLoop* startLoop();

  private:
    void threadFunc();

    EventLoop* loop_;
    bool exiting;
    Thread thread_;
    std::mutex mutex_;
    std::condition_variable cond_;
    // ThreadInitCallback threadinitcallback_;

};

#endif