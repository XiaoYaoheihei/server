#include "Epoll.h"
#include "../net/poller.h"
#include "../net/Channel.h"
#include "../base/Timestamp.h"

Epoll::Epoll(EventLoop* loop) 
  : owner(loop),
    epollfd_(::epoll_create1(EPOLL_CLOEXEC)),
    events_(16) {}

Timestamp Epoll::poll(ChannelList* activeChannels, int times) {
  int numEvents = ::epoll_wait(epollfd_, &*events_.begin(),
                               static_cast<int>(events_.size()), times);
  Timestamp timestamp;
  if (numEvents > 0) {
    std::cout << "有事件触发" << " " << numEvents << std::endl;
    fillactiveChannels(numEvents, activeChannels);
    if (static_cast<size_t>(numEvents) == events_.size()) {
      events_.resize(events_.size()*2);
    }
  }

  return timestamp;
}

void Epoll::fillactiveChannels(int numEvents, 
                               ChannelList* activeChannels) {
  for (int i = 0; i < numEvents; i++) {
    //将void*类型转化成Channel*类型
    Channel* channel = static_cast<Channel*>(events_[i].data.ptr);
    channel->setRevent(events_[i].events);
    activeChannels->push_back(channel);
  }
}

void Epoll::updateChannel(Channel* chan) {
  int index = chan->getPollIdx();
  int fd = chan->getFd();
  if (index == -1 || index == 2) {
    if (index == -1) {//第一次向Epoll中更新
      channels_[fd] = chan;
      //打印ChannelMap的大小
      // LOG_DEBUG<<channels_.size();
    } else {

    }
    chan->setPollIdx(1);
    update(EPOLL_CTL_ADD, chan);
  } else {
    if (chan->Is_noneEvent()) {
      //此时的channel没有需要关注的事件,删除相关
      update(EPOLL_CTL_DEL, chan);
      chan->setPollIdx(2);
    } else {
      update(EPOLL_CTL_MOD, chan);
    }
  }
}

void Epoll::update(int mode, Channel* chan) {
  //epoll事件
  struct epoll_event event;
  event.events = chan->getEvent();
  event.data.ptr = chan;
  int fd = chan->getFd();
  ::epoll_ctl(epollfd_, mode, fd, &event);
}

void Epoll::removeChannel(Channel* channel) {
  int fd = channel->getFd();
  int index = channel->getPollIdx();
  size_t n = channels_.erase(fd);
  // LOG_DEBUG<<channels_.size();
  if (index == 1) {
    update(EPOLL_CTL_DEL, channel);
  }
  channel->setPollIdx(-1);
}