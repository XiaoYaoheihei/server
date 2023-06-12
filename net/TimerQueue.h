#ifndef NET_TIMERQUEUE_H
#define NET_TIMERQUEUE_H
#include "Timer.h"
#include "Channel.h"
#include <memory>
#include <queue>

//自定义的仿函数
struct TimerCmp
{
  bool operator()(std::shared_ptr<Timer>& a, std::shared_ptr<Timer>& b) {
    if (a->getExpiration() == b->getExpiration() ) {
      //小顶堆，按照序号的从小到大来进行排列
      return a->getSequence() > b->getSequence();
    } else {
      //小顶堆，按照期望的时间顺序从小到大进行排列
      return a->getExpiration() > b->getExpiration();
    }
  }
};


class EventLoop;
//定时器队列
class TimerQueue {
  public:
    //定时器的优先队列，定时器按照时间顺序从小到大的顺序进行排列
    using TimerList = std::priority_queue<std::shared_ptr<Timer>, std::vector<std::shared_ptr<Timer>>, TimerCmp>;
    TimerQueue(EventLoop* loop);
    ~TimerQueue();

    //添加定时器任务
    std::weak_ptr<Timer> addTimer(TimeCallback cb, Timer::TimePoint when, Timer::TimeUnit dur);
    //取消
    //cansel()
  private:
    //添加定时器任务到优先队列当中去
    void addTimerInLoop(const std::shared_ptr<Timer>& timer);
    
    //处理回调事件
    void handleRead();
    //获取相应的Timer
    //从timers_中移除已经到期的Timer,并且通过vector返回他们
    std::vector<std::shared_ptr<Timer>> getExpired(const Timer::TimePoint& now);
    //重置Timer
    void reset(const std::vector<std::shared_ptr<Timer>> &expired, const Timer::TimePoint& now);
    //插入到优先队列中去
    bool insert(const std::shared_ptr<Timer> &timer);

    EventLoop* loop_;
    const int timerfd;
    Channel timerfdchannel;
    TimerList timers_;
};

#endif