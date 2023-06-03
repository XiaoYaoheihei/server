#include "../net/poller.h"
#include "../net/Channel.h"
#include <poll.h>
#include <iostream>

Poller::Poller(EventLoop* loop) 
  :owner(loop) {

} 

Poller::~Poller() {

}

void Poller::poll(ChannelList* activeChannels, int times) {
  //阻塞循环
  int numEvents = ::poll(PollFdList.data(), PollFdList.size(), times);
  if (numEvents > 0) {
    std::cout << "有事件触发" << " " << numEvents << std::endl;
    fillactiveChannels(numEvents, activeChannels);
  } 

  //日志本应该记录此时的时间
  // std::cout << "事件填充完毕" << std::endl;
}


void Poller::fillactiveChannels(int numEvents, ChannelList* activeChannels) {
  for (auto event: PollFdList) {
    if (numEvents == 0) break;
    if (event.revents > 0) {
      std::cout << event.revents << std::endl;
      auto ch = ChannelMap.find(event.fd);
      Channel* channel = ch->second;
      std::cout << ch->first << std::endl;
      std::cout << "此时的fd" << event.fd << std::endl;
      channel->setRevent(event.revents);
      std::cout << "no problem" << std::endl;
      activeChannels->push_back(channel);
      --numEvents;
      std::cout << "add success" << std::endl;
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
    ChannelMap[event.fd] = channel;

    //打印日志信息
    std::cout << "fd:" << channel->getFd() << " pidx:" << channel->getPollIdx()
              << " " << PollFdList.size() << " " << std::endl;

  } else {
    //已有的文件描述符
    int idx = channel->getPollIdx();
    struct pollfd& event = PollFdList[idx];
    event.events = channel->getEvent();
    event.fd = channel->getFd();
    event.revents = 0;

  }

  
}