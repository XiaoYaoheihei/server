#ifndef NET_EVENTLOOP_H
#define NET_EVENTLOOP_H

#include <vector>
#include <memory>

class Channel;
class Poller;

class EventLoop{
  public:
    EventLoop();
    ~EventLoop();

    void loop();
    void quit();
    void updateChannel(Channel* channel);
  private:
    //活跃的事件表
    typedef std::vector<Channel*> ChannelList;
    ChannelList activeChannels;

    bool looping;
    bool quit_;
    //这里的Poller还待改进
    std::unique_ptr<Poller> poller_;

};


#endif