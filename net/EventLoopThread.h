#ifndef NET_EVENTLOOPTHREAD_H
#define NET_EVENTLOOPTHREAD_H
#include <mutex>
#include <condition_variable>
#include "Callbacks.h"
#include "../base/Thread.h"

class EventLoop;
class EventLoopThread {
  public:
    EventLoopThread(const ThreadInitCallback& callback, const std::string& name);
    ~EventLoopThread();
    EventLoop* startLoop();

  private:
    void threadFunc();

    EventLoop* loop_;
    bool exiting;
    Thread thread_;
    std::mutex mutex_;
    std::condition_variable cond_;
    ThreadInitCallback threadinitcallback_;

};

#endif