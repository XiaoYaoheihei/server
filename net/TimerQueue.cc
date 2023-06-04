#include "../net/TimerQueue.h"
#include "../net/Channel.h"
#include <sys/timerfd.h>

int creatTimerFd() {
  int timefd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);

  if (timefd < 0) {
    //打印日志，timerfd创建失败
  }
  return timefd;
}


TimerQueue::TimerQueue(EventLoop* loop) 
  : loop_(loop),
    timerfd(creatTimerFd()),
    timerfdchannel(loop_, timerfd)

    {
      timerfdchannel.setRead(std::bind(&TimerQueue::handleRead, this));
      timerfdchannel.enableReading();
}

std::weak_ptr<Timer> TimerQueue::addTimer(TimeCallback cb,
                                          Timer::TimePoint when,
                                          Timer::TimeUnit dur) {
  
}

//处理回调事件
void TimerQueue::handleRead() {

}