#ifndef NET_TIMERQUEUE_H
#define NET_TIMERQUEUE_H
#include "Timer.h"
#include "Channel.h"
#include <memory>

class EventLoop;
//定时器队列
class TimerQueue {
  public:
    TimerQueue(EventLoop* loop);
    ~TimerQueue();

    //添加定时器任务
    std::weak_ptr<Timer> addTimer(TimeCallback cb, Timer::TimePoint when, Timer::TimeUnit dur);

  private:
    
    
    //处理回调事件
    void handleRead();
    //获取相应的Timer
    std::vector<std::shared_ptr<Timer>> getExpired(const Timer::TimePoint& now);
    void reset(const std::vector<std::shared_ptr<Timer>> &expired, const Timer::TimePoint& now);
    //
    bool insert(const std::shared_ptr<Timer> &timer);

    EventLoop* loop_;
    Channel timerfdchannel;
    int timerfd;
};

#endif