#ifndef NET_CALLBACKS_H
#define NET_CALLBACKS_H

#include <functional>


//定时器回调函数
typedef std::function<void()> TimeCallback;

typedef std::function<void()> Functor;
#endif