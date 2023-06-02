#include "../net/poller.h"
#include "../net/Channel.h"
#include <poll.h>
#include <iostream>

Poller::Poller(EventLoop* loop) 
  :owner(loop) {

} 

Poller::~Poller() {

}

void Poller::poll(ChannelList* activeChannels) {
  //阻塞循环
  int numEvents = ::poll(PollFdList.data(), PollFdList.size(), 0);
  if (numEvents > 0) {
    fillactiveChannels(numEvents, activeChannels);
  } else if (numEvents == 0) {

  } else {

  }
}


void Poller::fillactiveChannels(int numEvents, ChannelList* activeChannels) {
  for (auto event: PollFdList) {
    if (numEvents == 0) break;
    if (event.revents > 0) {
      
      auto ch = ChannelMap.find(event.fd);
      Channel* channel = ch->second;
      std::cout << "此时的fd" << std::endl;
      channel->setRevent(event.revents);
      activeChannels->push_back(channel);
      --numEvents;
    }
  }
}

void Poller::updateChannel(Channel* channel) {
  //添加新的文件描述符
  if (channel->getIndex() < 0) {
    struct pollfd event;
    event.fd = channel->getFd();
    event.events = channel->getEvent();
    event.revents = 0;

    PollFdList.push_back(event);
    int idx = PollFdList.size()-1;
    //记录channel在pollfdlist中的位置
    channel->setPollIdx(idx);
    ChannelMap[idx] = channel;

  } else {
    //已有的文件描述符
    int idx = channel->getPollIdx();
    struct pollfd& event = PollFdList[idx];
    event.events = channel->getEvent();
    event.fd = channel->getFd();
    event.revents = 0;

  }

  
}