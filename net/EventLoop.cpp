#include <iostream>
#include "../net/EventLoop.h"
#include "../net/poller.h"
#include "../net/Channel.h"

const int PollTimeMs = 10000;

EventLoop::EventLoop()
  : poller_(std::make_unique<Poller>(this)),
    quit_(false) {

}

EventLoop::~EventLoop() {

}

//事件循环检测
void EventLoop::loop() {
  looping = true;

  //添加日志信息
  std::cout << "EventLoop start to loop" << std::endl;

  while (!quit_) {
    activeChannels.clear();

    poller_->poll(&activeChannels, PollTimeMs);

    for (auto channel : activeChannels) {
      //日志信息
      std::cout << "start to callback" << std::endl;
      channel->handleEvent();
    }
  }

  quit_ = true;
}

void EventLoop::quit() {
  quit_ = true;
}

void EventLoop::updateChannel(Channel* channel) {
  poller_->updateChannel(channel);
} 