#ifndef NET_TIMER_H
#define NET_TIMER_H
#include <chrono>
#include "Callbacks.h"

//定时器
class Timer{
  public:
    //时间间隔
    using TimeUnit = std::chrono::nanoseconds;
    //时间点
    using TimePoint = std::chrono::steady_clock::time_point;
    Timer(TimeCallback cb, TimePoint when, TimeUnit dur)
      : callback(std::move(cb)),
        expiration(when),
        interval(dur),
        del(false),
        rep(),
        sequence(numCreated++) {};
    
    void run() const {
      callback();
    }

    TimePoint getExpiration() const {
      return expiration;
    }
    bool getDel() const {
      return del;
    }
    bool getRep() const {
      return rep;
    }
    int getSequence() const {
      return sequence;
    }
    //重置时间
    void restart(const Timer::TimePoint& now) {
      expiration = now + interval;
    }
  private:
    TimeCallback callback;
    TimePoint expiration;
    TimeUnit interval;

    bool del;//是否已经移除
    bool rep;//是否有重复
    int sequence;//顺序

    static int numCreated;//静态变量来表示已经创建的Timer数量
};

#endif