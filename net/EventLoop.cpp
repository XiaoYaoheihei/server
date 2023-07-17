#include <iostream>
#include <sys/syscall.h>
#include <unistd.h>
#include <sys/eventfd.h>
#include <algorithm>

#include "../net/EventLoop.h"
#include "../net/poller.h"
#include "../net/Channel.h"
#include "../net/TimerQueue.h"
#include "../base/Timestamp.h"

const int PollTimeMs = 100;

int createEventfd() {
  int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
  if (evtfd < 0) {
    // LOG_SYSERR << "Failed in eventfd";
    abort();
  }
  return evtfd;
}

EventLoop::EventLoop()
  : looping(false),
    quit_(false),
    //syscall的这种写法
    threadId(syscall(SYS_gettid)),
    eventHanding(false),
    callingPendingFunctors(false),
    poller_(std::make_unique<Poller>(this)),
    wakeupFd(createEventfd()),
    wakeupChannel(std::make_unique<Channel>(this, wakeupFd)),
    timerQueue_(std::make_unique<TimerQueue>(this)) {
      wakeupChannel->setRead(std::bind(&EventLoop::handleRead, this));
      //一直从wakeupFd中读取信息
      wakeupChannel->enableReading();
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

    Timestamp ret_ = poller_->poll(&activeChannels, PollTimeMs);
    eventHanding = true;
    for (auto channel : activeChannels) {
      //日志信息
      // std::cout << "start to callback" << std::endl;
      channel->handleEvent(ret_);
    }
    eventHanding = false;
    doPendingFunctors();
  }

  quit_ = true;
}

void EventLoop::quit() {
  quit_ = true;
  if (!isInloopThread()) {
    wakeup();
  }
}

void EventLoop::updateChannel(Channel* channel) {
  poller_->updateChannel(channel);
} 

void EventLoop::removeChannel(Channel* channel) {
  if (eventHanding) {
    //目前不知道这里是干什么的？？？
    std::find(activeChannels.begin(), activeChannels.end(), channel);
  }

  poller_->removeChannel(channel);
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
  //打印相关日志
  return threadId == syscall(SYS_gettid);
}

void EventLoop::runInLoop(const Functor& callback) {
  std::cout << syscall(SYS_gettid);
  std::cout << " " << threadId << std::endl;
  //如果该EventLoop是IO线程，直接执行
  if (isInloopThread()) {
    //打印日志，直接执行
    callback();
  } else {
    //不是IO线程的话，异步执行
    //打印日志，采取异步执行的方式
    queueInLoop(callback);
  }
}

void EventLoop::queueInLoop(const Functor& callback) {
  {
    std::lock_guard<std::mutex> lock(mutex_);
    pendingFunctors.push_back(callback);
  }
  if (!isInloopThread() || callingPendingFunctors) {
    wakeup();
  }
}

void EventLoop::wakeup() {
  uint64_t one = 1;
  ssize_t n = write(wakeupFd, &one, sizeof(one));
}

void EventLoop::handleRead() {
  uint64_t one = 1;
  ssize_t n = read(wakeupFd, &one, sizeof(one));
}

void EventLoop::doPendingFunctors() {
  std::vector<Functor> functors;
  callingPendingFunctors = true;
  {
    std::lock_guard<std::mutex> lock(mutex_);
    functors.swap(pendingFunctors);
  }

  for (const Functor& functor : functors) {
    functor();
  }
  callingPendingFunctors = false;
}