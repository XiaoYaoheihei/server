#include "EventLoop.h"
#include "EventLoopThread.h"
#include "EventLoopThreadPool.h"

EventLoopThreadPool::EventLoopThreadPool(EventLoop* baseloop, const std::string& name)
  : baseloop_(baseloop),
    name_(name),
    started_(false),
    numThreads_(10),
    next_(0) {}

EventLoopThreadPool::~EventLoopThreadPool() {

}

void EventLoopThreadPool::start(const ThreadInitCallback& callback) {
  //打印日志消息

  started_ = true;
  for (int i = 0; i < numThreads_; i++) {
    //对每一个Thread命名
    char buf[name_.size() + 32];
    snprintf(buf, sizeof buf, "%s%d", name_.c_str(), i);
    std::cout << "start to build" << std::endl;
    EventLoopThread* tmp = new EventLoopThread(callback, buf);
    threads_.push_back(std::unique_ptr<EventLoopThread>(tmp));
    //获取EventLoop对象
    loops_.push_back(tmp->startLoop());
  }
  //如果线程数==0并且回调存在的话调用回调
  if (numThreads_ == 0 && callback) {
    callback(baseloop_);
  }
}

//获取Pool中的任何一个EventLoop
EventLoop* EventLoopThreadPool::getnextLoop() {
  EventLoop* loop = baseloop_;
  if (!loops_.empty()) {
    //loops_中有存在的loop对象，通过下标访问
    loop = loops_[next_];
    ++next_;
    if (next_ >= loops_.size()) {
      next_ = 0;
    }
  }
  return loop;
}

std::vector<EventLoop*> EventLoopThreadPool::getallLoops() {
  if (loops_.empty()) {
    return std::vector<EventLoop*>(1, baseloop_);
  } else {
    return loops_;
  }
}

const std::string& EventLoopThreadPool::getname() const {
  return name_;
}

void EventLoopThreadPool::setThreadNum(int numThread) {
  numThreads_ = numThread;
}

bool EventLoopThreadPool::started() const {
  return started_;
}
