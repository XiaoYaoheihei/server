#ifndef NET_EVENTLOOP_H
#define NET_EVENTLOOP_H

#include <vector>
#include <memory>
#include <chrono>
#include <mutex>
#include "Callbacks.h"

class Channel;
class Poller;
class Timer;
class TimerQueue;

class EventLoop{
  public:
    EventLoop();
    ~EventLoop();

    bool isInloopThread();

    void loop();
    void quit();
    void updateChannel(Channel* channel);
    void wakeup();

    //把任务放到IO线程中执行
    void runInLoop(const Functor& callback);
    void queueInLoop(const Functor& callback);
    
    //timer
    std::weak_ptr<Timer> runAt(const std::chrono::steady_clock::time_point& time, TimeCallback cb);
    std::weak_ptr<Timer> runAfter(const std::chrono::nanoseconds& delay, TimeCallback cb);
    std::weak_ptr<Timer> runEvery(const std::chrono::nanoseconds& interval, TimeCallback cb);
  private:
    void doPendingFunctors();
    void handleRead();

    //活跃的事件表
    typedef std::vector<Channel*> ChannelList;
    ChannelList activeChannels;

    bool looping;
    bool quit_;
    const pid_t threadId;
    bool eventHanding;
    bool callingPendingFunctors;
    //这里的Poller还待改进
    std::unique_ptr<Poller> poller_;
    
    int wakeupFd;
    std::unique_ptr<Channel> wakeupChannel;

    std::mutex mutex_;
    std::vector<Functor> pendingFunctors;

    std::unique_ptr<TimerQueue> timerQueue_;

};


#endif