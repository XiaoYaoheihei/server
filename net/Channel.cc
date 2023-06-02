#include "../net/Channel.h"
#include "../net/EventLoop.h"
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
    index(-1) {


}

//由EventLoop的loop调用，根据runningEvent的值来调用不同的回调函数
void Channel::handleEvent() {

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

}

void Channel::update() {
  loop_->updateChannel(this);
}


void Channel::setRead(const EventCallback& cb) {
  readcallback = cb;
}

void Channel::setWrite(const EventCallback& cb) {
  writecallback = cb;
}
    
void Channel::Channel::setError(const EventCallback& cb) {
  errorcallback = cb;
}

void Channel::setRevent(int event) {
  runningEvent = event;
}
int Channel::getIndex() {
  return index;
}
void Channel::setIndex(int number) {
  index = number;
}
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
}