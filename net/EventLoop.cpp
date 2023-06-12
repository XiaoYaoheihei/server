#include <iostream>
#include <sys/syscall.h>
#include <unistd.h>
#include <algorithm>

#include "../net/EventLoop.h"
#include "../net/poller.h"
#include "../net/Channel.h"
#include "../net/TimerQueue.h"

const int PollTimeMs = 100;

EventLoop::EventLoop()
  : looping(false),
    quit_(false),
    threadId(syscall(SYS_gettid)),
    poller_(std::make_unique<Poller>(this)),
    timerQueue_(std::make_unique<TimerQueue>(this)) {

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

//在某一时刻回调定时器任务
std::weak_ptr<Timer> EventLoop::runAt(const std::chrono::steady_clock::time_point& time, TimeCallback cb) {
  return timerQueue_->addTimer(std::move(cb), time, Timer::TimeUnit());
}

//延迟执行回调
std::weak_ptr<Timer> EventLoop::runAfter(const std::chrono::nanoseconds& delay, TimeCallback cb) {
  Timer::TimePoint time = std::chrono::steady_clock::now() + delay;
  return timerQueue_->addTimer(cb, time, Timer::TimeUnit());
}

std::weak_ptr<Timer> EventLoop::runEvery(const std::chrono::nanoseconds& interval, TimeCallback cb) {
  Timer::TimePoint time = std::chrono::steady_clock::now() + interval;
  return timerQueue_->addTimer(cb, time, Timer::TimeUnit());
}

bool EventLoop::isInloopThread() {
  return threadId == syscall(SYS_gettid);
}

void EventLoop::runInLoop(const Functor& callback) {
  if (isInloopThread()) {
    callback();
  } else {
    queueInLoop(callback);
  }
}