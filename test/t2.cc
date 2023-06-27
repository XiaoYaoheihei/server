#include "../net/EventLoop.h"
#include <stdio.h>
#include <iostream>

EventLoop* g_loop;
int cnt = 0;

void print() {
  printf("cnt = %d\n", cnt);
  if (++cnt == 20) {
    g_loop->quit();
  }
}

int main() {
  EventLoop loop;
  g_loop = &loop;

  printf("mian\n");
  auto time = std::chrono::nanoseconds(1);
  loop.runAfter(time, print);
  time = std::chrono::nanoseconds(2);
  loop.runEvery(time, print);
  time = std::chrono::nanoseconds(3);
  loop.runEvery(time, print);

  loop.loop();
  printf("loop exits\n");

}
