#include "../net/poller.h"
#include "../net/Channel.h"
#include "../base/Timestamp.h"
#include <poll.h>
#include <iostream>

Poller::Poller(EventLoop* loop) 
  :owner(loop) {

} 

Poller::~Poller() {

}

Timestamp Poller::poll(ChannelList* activeChannels, int times) {
  //阻塞循环
  int numEvents = ::poll(PollFdList.data(), PollFdList.size(), times);
  Timestamp timestamp;
  if (numEvents > 0) {
    std::cout << "有事件触发" << " " << numEvents << std::endl;
    fillactiveChannels(numEvents, activeChannels);
  } 

  //日志本应该记录此时的时间
  // std::cout << "事件填充完毕" << std::endl;
  return timestamp;
}


void Poller::fillactiveChannels(int numEvents, ChannelList* activeChannels) {
  for (auto event: PollFdList) {
    if (numEvents == 0) break;
    if (event.revents > 0) {
      auto ch = ChannelMap.find(event.fd);
      Channel* channel = ch->second;
      std::cout << "此时的fd:" << event.fd << std::endl;
      channel->setRevent(event.revents);
      activeChannels->push_back(channel);
      --numEvents;
    }
  }
}

void Poller::updateChannel(Channel* channel) {
  //添加新的文件描述符
  if (channel->getPollIdx() < 0) {
    struct pollfd event;
    event.fd = channel->getFd();
    // std::cout << channel->getFd() << "poller" << std::endl;
    event.events = channel->getEvent();
    event.revents = 0;

    PollFdList.push_back(event);
    int idx = PollFdList.size()-1;
    //记录channel在pollfdlist中的位置
    channel->setPollIdx(idx);
    ChannelMap[event.fd] = channel;

    //打印日志信息
    std::cout << "添加的情况\nfd:" << channel->getFd() << " pidx:" << channel->getPollIdx()
              << " " << PollFdList.size() << " " << std::endl;

  } else {
    //已有的文件描述符
    int idx = channel->getPollIdx();
    //通过引用的方式直接进行修改
    struct pollfd& event = PollFdList[idx];
    event.events = channel->getEvent();
    event.fd = channel->getFd();
    event.revents = 0;

    if (channel->Is_noneEvent()) {
      event.fd = -1;
    }
  }
}

//从poll中移除对应的channel
void Poller::removeChannel(Channel* chan) {
  int idx = chan->getPollIdx();
  std::cout << "before erase:" << PollFdList.size() << std::endl;
  struct pollfd& event = PollFdList[idx];
  int n = ChannelMap.erase(chan->getFd());
  //打印相关日志，检查此时是否已经删除成功
// LOG_DEBUG << idx << " " << PollFdList.size();
  if (static_cast<size_t>(idx) == PollFdList.size()-1) {
    PollFdList.pop_back();
  } else {
    //将待删除的元素与最后一个元素交换，再去pop_back
    int channelAtEnd = PollFdList.back().fd;
    //交换
    iter_swap(PollFdList.begin() + idx, PollFdList.end()-1);
    //这里的<0是干什么的？？？
    if (channelAtEnd < 0) {
      channelAtEnd = -channelAtEnd-1;
    }
    //重新设置交换后的下标
    ChannelMap[channelAtEnd]->setPollIdx(idx);
    PollFdList.pop_back();
  }
  std::cout << "after erase:" << PollFdList.size() << std::endl;
}