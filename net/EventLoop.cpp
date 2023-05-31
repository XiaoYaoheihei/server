#include <iostream>
#include "../net/EventLoop.h"



void EventLoop::loop() {
  looping = true;

  //添加日志信息
  std::cout << "EventLoop start to loop" << std::endl;

  quit = true;
}

