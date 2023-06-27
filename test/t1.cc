#include <iostream>
#include <sys/timerfd.h>
#include <stdio.h>
#include <unistd.h>
#include <strings.h>
#include "../net/EventLoop.h"
#include "../net/Channel.h"

EventLoop* g_loop;
void timeout() {
  std::cout << "time out" << std::endl;

  g_loop->quit();
}


int main() {
  EventLoop loop;
  g_loop = &loop;

  int timefd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
  // std::cout << timefd << std::endl;
  Channel chan(&loop, timefd);
  chan.setRead(timeout);
  chan.enableReading();

  struct itimerspec howlong;
  bzero(&howlong, sizeof howlong);
  howlong.it_value.tv_sec = 5;
  timerfd_settime(timefd, 0, &howlong, NULL);    


  loop.loop();
  close(timefd);
  return 0;
}