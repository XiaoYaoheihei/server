#include "../net/Channel.h"

Channel::Channel(EventLoop* loop, int fd) 
  : loop_(loop),
    fd_(fd),
    events(0),
    runningEvent(0) {


}

//由EventLoop的loop调用，根据runningEvent的值来调用不同的回调函数
void Channel::handleEvent() {

}