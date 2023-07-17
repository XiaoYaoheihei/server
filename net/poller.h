#ifndef NET_POLLER_H
#define NET_POLLER_H

#include <vector>
#include <map>
#include "../base/Timestamp.h"

class EventLoop;
class Channel;
typedef std::vector<Channel*> ChannelList;
class Poller {
  public:
    
    Poller(EventLoop* loop);
    ~Poller();

    //仅供EventLoop调用
    Timestamp poll(ChannelList* activeChannels, int times);
    void updateChannel(Channel* chan);
    void removeChannel(Channel* chan);
    
  private:
    void fillactiveChannels(int numEvents, 
                            ChannelList* activeChannels);

    EventLoop* owner;
    std::vector<struct pollfd> PollFdList;
    std::map<int, Channel*> ChannelMap;
    
};


#endif