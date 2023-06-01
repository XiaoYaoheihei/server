#include <iostream>
#include "../net/EventLoop.h"
#include "../net/poller.h"
#include "../net/Channel.h"

EventLoop::EventLoop()
  : poller_(*(new Poller(this))),
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
    poller_.poll(&activeChannels);

    for (auto event : activeChannels) {
      //日志信息

      event->handleEvent();
    }
  }

  quit_ = true;
}

void EventLoop::quit() {
  quit_ = true;
}
