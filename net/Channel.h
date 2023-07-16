#ifndef NET_CHANNEL_H
#define NET_CHANNEL_H

#include <functional>

class EventLoop;
class Channel{
  public:
    typedef std::function<void()> EventCallback;
    
    Channel(EventLoop* loop, int fd);
    ~Channel() = default;
    void handleEvent(); 

    void remove();

    //回调函数具体赋值，cb是函数指针
    void setRead(const EventCallback& cb);
    void setWrite(const EventCallback& cb); 
    void setError(const EventCallback& cb);
    void setClose(const EventCallback& cb);
    
    void setRevent(short event);
    int getIndex(); 
    void setIndex(int number); 
    int getFd(); 
    int getEvent(); 
    void setPollIdx(int number); 
    int getPollIdx(); 

    void enableReading(); 
    void disableAll();


    bool Is_noneEvent();

  private:
    void update();

    //channel关心的事件
    int events;     
    //目前活动的事件
    short runningEvent;
    EventLoop* loop_;
    const int fd_;
    // int index;
    int pollidx;

    EventCallback readcallback;
    EventCallback writecallback;
    EventCallback errorcallback;
    EventCallback closecallback;

    static const int NoEvent;
    static const int ReadEvent;
    static const int WriteEvent;

    bool eventHanding;
};

#endif