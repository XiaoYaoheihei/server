#ifndef NET_POLLER_H
#define NET_POLLER_H

#include <vector>
#include <map>

class EventLoop;
class Channel;
class Poller {
  public:
    typedef std::vector<Channel*> ChannelList;

    Poller(EventLoop* loop);
    ~Poller()=default;

    //仅供EventLoop调用
    void poll(ChannelList* activeChannels);
    void updateChannel(Channel* chan);

  private:
    void fillactiveChannels(int numEvents, 
                            ChannelList* activeChannels);

    EventLoop* owner;
    std::vector<struct pollfd> PollFdList;
    std::map<int, Channel*> ChannelMap;
    
};


#endif