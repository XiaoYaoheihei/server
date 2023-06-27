#include "../net/EventLoop.h"
#include "../net/EventLoopThread.h"
#include <stdio.h>
#include <unistd.h>

void runInThread()
{
  printf("runInThread(): pid = %d\n",
         getpid());
}

int main()
{
  printf("main(): tid = %d\n",
         gettid());

  EventLoopThread loopThread;
  //在线程b中创建了loop对象，在线程a中得到了loop对象的指针
  //创建loop对象的应该是IO线程
  EventLoop* loop = loopThread.startLoop();
  //在线程a中执行相应的函数
  
  loop->runInLoop(runInThread);
  sleep(1);
  auto time = std::chrono::nanoseconds(1);
  loop->runAfter(time, runInThread);
  sleep(3);
  loop->quit();

  printf("exit main().\n");
}