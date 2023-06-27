#include "EventLoopThread.h"
#include "EventLoop.h"
// IO线程不一定是主线程
// 在其他线程中创建EventLooop对象，主线程只是获取创建的对象

EventLoopThread::EventLoopThread(const std::string& name) 
  : loop_(NULL),
    exiting(false),
    //新建一个线程专门用来创建EventLoop对象
    thread_(std::bind(&EventLoopThread::threadFunc, this))
    // threadinitcallback_() 
    {
}

EventLoopThread::~EventLoopThread() {

}

//启动自己的线程,返回新线程中的EventLoop对象
//使用条件变量等待线程的创建与运行
EventLoop* EventLoopThread::startLoop() {
  thread_.start();
  EventLoop* loop = nullptr;
  {
    std::unique_lock<std::mutex> lock(mutex_);
    while (loop_ == nullptr) {
      //等待线程创建EventLoop对象
      cond_.wait(lock);
    }
    loop = loop_;
  }
  return loop;
}

//给线程绑定的函数
//在stack上定义EventLoop对象，然后将其赋值给loop_对象
//最后唤醒条件变量
void EventLoopThread::threadFunc() {
  EventLoop loop;
  {
    std::unique_lock<std::mutex> lock(mutex_);
    loop_ = &loop;
    cond_.notify_one();
  }
  loop.loop();

  std::unique_lock<std::mutex> lock(mutex_);
  loop_ = nullptr;
}