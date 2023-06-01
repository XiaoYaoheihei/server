#ifndef NET_CHANNEL_H
#define NET_CHANNEL_H

#include <functional>

class EventLoop;
class Channel{
  public:
    typedef std::function<void()> EventCallback;
    
    Channel(EventLoop* loop, int fd);
    ~Channel();
    void handleEvent(); 

    //回调函数具体赋值，cb是函数指针
    void setread(const EventCallback& cb) {
      readcallback = cb;
    }
    void setwrite(const EventCallback& cb) {
      writecallback = cb;
    }
    void seterror(const EventCallback& cb) {
      errorcallback = cb;
    }

  private:

    //channel关心的事件
    int events;     
    //目前活动的事件
    int runningEvent;
    EventLoop* loop_;
    const int fd_;

    EventCallback readcallback;
    EventCallback writecallback;
    EventCallback errorcallback;
};

#endif