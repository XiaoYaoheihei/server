#include "../net/EventLoop.h"
#include <unistd.h>
#include <stdio.h>

EventLoop* g_loop;
int g_flag = 0;

void run4()
{
  printf("run4(): pid = %d, flag = %d\n", getpid(), g_flag);
  g_loop->quit();
}

void run3()
{
  printf("run3(): pid = %d, flag = %d\n", getpid(), g_flag);
  auto now = std::chrono::nanoseconds(3);
  g_loop->runAfter(now, run4);
  g_flag = 3;
}

void run2()
{
  printf("run2(): pid = %d, flag = %d\n", getpid(), g_flag);
  g_loop->queueInLoop(run3);
}

void run1()                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       
{
  g_flag = 1;
  printf("run1(): pid = %d, flag = %d\n", getpid(), g_flag);
  g_loop->runInLoop(run2);
  g_flag = 2;
}

int main()
{
  printf("main(): pid = %d, flag = %d\n", getpid(), g_flag);

  EventLoop loop;
  g_loop = &loop;

  auto time = std::chrono::nanoseconds(2);
  loop.runAfter(time, run1);
  loop.loop();
  printf("main(): pid = %d, flag = %d\n", getpid(), g_flag);
}