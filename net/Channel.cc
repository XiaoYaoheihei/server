#include "../net/Channel.h"
#include "../net/EventLoop.h"
#include <iostream>
#include <poll.h>

//事件掩码来判断事件类型
const int Channel::NoEvent = 0;
const int Channel::ReadEvent = POLLIN | POLLPRI;
const int Channel::WriteEvent = POLLOUT;

Channel::Channel(EventLoop* loop, int fd) 
  : loop_(loop),
    fd_(fd),
    events(0),
    runningEvent(0),
    pollidx(-1),
    eventHanding(false) {
      // std::cout << this->getFd() << "channel timerfd"<< std::endl;


}

//由EventLoop的loop调用，根据runningEvent的值来调用不同的回调函数
void Channel::handleEvent() {
  std::cout << "now running events is:" << this->runningEvent << std::endl;
  eventHanding = true;
  if (runningEvent & (POLLERR | POLLNVAL)) {
    //有错误函数回调的话
    if (errorcallback) {
      errorcallback();
    }
  }
  if (runningEvent & (POLLIN | POLLPRI | POLLRDHUP)) {
    if (readcallback) {
      readcallback();
    }
  }
  if (runningEvent & POLLOUT) {
    if (writecallback) {
      writecallback();
    }
  }
  if ((runningEvent & POLLHUP ) && !(runningEvent & POLLIN)) {
    if (closecallback) {
      closecallback();
    }
  }
  eventHanding = false;
}

void Channel::update() {
  loop_->updateChannel(this);
}

void Channel::remove() {
  loop_->removeChannel(this);
}


void Channel::setRead(const EventCallback& cb) {
  readcallback = cb;
}

void Channel::setWrite(const EventCallback& cb) {
  writecallback = cb;
}

void Channel::setClose(const EventCallback& cb) {
  closecallback = cb;
}
    
void Channel::setError(const EventCallback& cb) {
  errorcallback = cb;
}

void Channel::setRevent(short event) {
  runningEvent = event;
}
// int Channel::getIndex() {
//   return index;
// }
// void Channel::setIndex(int number) {
//   index = number;
// }
int Channel::getFd() {
  return fd_;
}
int Channel::getEvent() {
  return events;
}
void Channel::setPollIdx(int number) {
  pollidx = number;
}
int Channel::getPollIdx() {
  return pollidx;
}

void Channel::enableReading() {
  events |= ReadEvent;
  update();
  // std::cout << this->getFd() << "timerfd"<< std::endl;
}

void Channel::disableAll() {
  //将events关心的事件的值设置为0
  events = NoEvent;
  update();
}

bool Channel::Is_noneEvent() {
  return !events;
}