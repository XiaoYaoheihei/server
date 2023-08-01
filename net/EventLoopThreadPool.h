#ifndef NET_EVENTLOOPTHREADPOLL_H
#define NET_EVENTLOOPTHREADPOLL_H

#include "Callbacks.h"
#include <iostream>

class EventLoop;
class EventLoopThread;
class EventLoopThreadPool {
  public:
    EventLoopThreadPool(EventLoop* baseloop, const std::string& name);
    ~EventLoopThreadPool();

    EventLoop* getnextLoop();
    std::vector<EventLoop*> getallLoops();

    const std::string& getname() const;
    bool started() const;

    void setThreadNum(int numThread);
    void start(const ThreadInitCallback& callback = ThreadInitCallback());

  private:
    EventLoop* baseloop_;
    std::string name_;
    bool started_;
    int numThreads_;
    //用来标识loops_中的下标位置
    int next_;
    std::vector<std::unique_ptr<EventLoopThread>> threads_;
    std::vector<EventLoop*> loops_;
};


#endif