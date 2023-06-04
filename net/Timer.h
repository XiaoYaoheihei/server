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
        interval(dur) {};
    
    void run() const {
      callback();
    }

    TimePoint getExpiration() const {
      return expiration;
    }

  private:
    TimeCallback callback;
    TimePoint expiration;
    TimeUnit interval;

};

#endif