#ifndef NET_EVENTLOOP_H
#define NET_EVENTLOOP_H

#include <vector>
#include <memory>
#include <chrono>
#include "Callbacks.h"

class Channel;
class Poller;
class Timer;
class TimerQueue;

class EventLoop{
  public:
    EventLoop();
    ~EventLoop();

    void loop();
    void quit();
    void updateChannel(Channel* channel);

    //timer
    std::weak_ptr<Timer> runAt(const std::chrono::steady_clock::time_point& time, TimeCallback cb);
    std::weak_ptr<Timer> runAfter(const std::chrono::nanoseconds& delay, TimeCallback cb);
    std::weak_ptr<Timer> runEvery(const std::chrono::nanoseconds& interval, TimeCallback cb);
  private:
    //活跃的事件表
    typedef std::vector<Channel*> ChannelList;
    ChannelList activeChannels;

    bool looping;
    bool quit_;
    //这里的Poller还待改进
    std::unique_ptr<Poller> poller_;
    
    std::unique_ptr<TimerQueue> timerQueue_;
};


#endif