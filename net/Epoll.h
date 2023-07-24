#ifndef NET_EPOLL_H
#define NET_EPOLL_H

#include <sys/epoll.h>
#include <vector>
#include <map>
#include "../base/Timestamp.h"


class EventLoop;
class Channel;
typedef std::vector<Channel*> ChannelList;
typedef std::map<int, Channel*> ChannelMap;
typedef std::vector<struct epoll_event> EventList;

class Epoll{
  public:
    Epoll(EventLoop* loop);
    ~Epoll();

    Timestamp poll(ChannelList* activeChannels, int times);
    void updateChannel(Channel* chan);
    void removeChannel(Channel* chan);

  private:
    //将events_中活动的事件填入activeChannels中
    void fillactiveChannels(int numEvents, 
                            ChannelList* activeChannels);
    void update(int mode, Channel* chan);

    EventLoop* owner;
    ChannelMap channels_;
    int epollfd_;
    //保存一次epoll_wait的时候活动的fd
    EventList events_;
};

#endif