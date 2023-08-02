#include "../net/Channel.h"
#include "../net/EventLoop.h"
#include "../base/Timestamp.h"
#include <iostream>
#include "../net/Epoll.h"
// #include <poll.h>

//事件掩码来判断事件类型
const int Channel::NoEvent = 0;
const int Channel::ReadEvent = EPOLLIN | EPOLLPRI;
const int Channel::WriteEvent = EPOLLOUT;

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
void Channel::handleEvent(Timestamp receivetime) {
  std::cout << "now running events is:" << this->runningEvent << std::endl;
  eventHanding = true;
  if (runningEvent & EPOLLERR) {
    //有错误函数回调的话
    if (errorcallback) {
      std::cout << "start to read errorcallback" << std::endl;
      errorcallback();
    }
  }
  //可读取事件，套接字对端关闭
  //ERR错误事件不应该放在这里
  if (runningEvent & (EPOLLIN | EPOLLPRI | EPOLLRDHUP )) {
    if (readcallback) {
      readcallback(receivetime);
    }
  }
  //数据可写事件
  if (runningEvent & EPOLLOUT) {
    if (writecallback) {
      writecallback();
    }
  }
  //出现挂断事件
  if (runningEvent & EPOLLHUP ) {
    if (closecallback) {
      closecallback();
    }
  }
  // if ((runningEvent & EPOLLHUP ) && !(runningEvent & EPOLLIN)) {
  //   if (closecallback) {
  //     closecallback();
  //   }
  // }
  eventHanding = false;
}

void Channel::update() {
  loop_->updateChannel(this);
}

void Channel::remove() {
  loop_->removeChannel(this);
}


void Channel::setRead(const ReadEventCallback& cb) {
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

//设置目前的活动事件
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

void Channel::enableWriting() {
  events |= WriteEvent;
  update();
}

void Channel::disableWriting() {
  events &= ~WriteEvent;
  update();
}

bool Channel::Is_noneEvent() {
  return !events;
}

bool Channel::Is_writing() {
  return events & WriteEvent;
}