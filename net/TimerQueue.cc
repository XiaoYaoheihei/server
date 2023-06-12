#include "../net/TimerQueue.h"
#include "../net/Channel.h"
#include <sys/timerfd.h>
#include <iostream>

int creatTimerFd() {
  int timefd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
  // std::cout << timefd << "timerfd"<< std::endl;
  if (timefd < 0) {
    //打印日志，timerfd创建失败
    std::cout << "print fail" << std::endl;
  }
  return timefd;
}


TimerQueue::TimerQueue(EventLoop* loop) 
  : loop_(loop),
    timerfd(creatTimerFd()),
    timerfdchannel(loop_, timerfd),
    timers_() {
      timerfdchannel.setRead(std::bind(&TimerQueue::handleRead, this));
      timerfdchannel.enableReading();
}

TimerQueue::~TimerQueue() {
  
}

std::weak_ptr<Timer> TimerQueue::addTimer(TimeCallback cb,
                                          Timer::TimePoint when,
                                          Timer::TimeUnit dur) {
  const std::shared_ptr<Timer> time = std::make_shared<Timer>(std::move(cb), when, dur);
  //lambda表达式的写法
  loop_->runInLoop([this, time]{ addTimerInLoop(time);});
  
  return time;
}

//处理回调事件
void TimerQueue::handleRead() {
  //获取当前时间
  std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
  //获取已经到时的定时器
  std::vector<std::shared_ptr<Timer>> expried = getExpired(now);
  for (auto& i : expried) {
    if (!i->getDel()) {
      //没有被标记为删除的话执行回调函数
      i->run();
    }
  }
  reset(expried, now);
}

//添加定时器任务到事件循环中去
void TimerQueue::addTimerInLoop(const std::shared_ptr<Timer>& timer) {
  bool ok = insert(timer);
  if (ok) {
    //打印日志显示添加到优先队列成功
    // std::cout << "add success " << std::endl;
  } else {
    //添加到优先队列失败
    // std::cout << "add fail" << std::endl;
  }
}

//获取已经到期的Timer
std::vector<std::shared_ptr<Timer>> TimerQueue::getExpired(const Timer::TimePoint& now) {
  std::vector<std::shared_ptr<Timer>> exp;
  //判断哪些定时器都已经超时了
  while (!timers_.empty() && timers_.top()->getExpiration() <= now) {
    //还没有被标记为删除的话
    if (!timers_.top()->getDel()) {
      //向后插入到vector中去
      exp.emplace_back(timers_.top());  
    }
    //从优先队列中pop
    timers_.pop();
  }
  return exp;
}


bool TimerQueue::insert(const std::shared_ptr<Timer>& timer) {
  bool ok = false;
  if (!timers_.empty()) {
    //定时器队列是非空的话
    timers_.push(timer);
    ok = true;
  } else {
    timers_.push(timer);
    ok = true;
  }
  return ok;
}

void TimerQueue::reset(const std::vector<std::shared_ptr<Timer>> &expired, const Timer::TimePoint& now) {
  for (auto& i : expired) {
    i->restart(now);
  }
}