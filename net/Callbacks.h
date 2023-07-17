#ifndef NET_CALLBACKS_H
#define NET_CALLBACKS_H
#include <memory>
#include <functional>
#include "Buffer.h"

class EventLoop;
class TcpConnection;
class Timestamp;

//定时器回调函数
typedef std::function<void()> TimeCallback;

typedef std::function<void()> Functor;

typedef std::function<void(EventLoop*)> ThreadInitCallback;

typedef std::function<void (int sockfd, const struct sockaddr_in&)>
  NewConnectionCallback;

typedef std::shared_ptr<TcpConnection> TcpConnectionptr;
typedef std::function<void (const TcpConnectionptr&)> ConnectionCallback;
typedef std::function<void (const TcpConnectionptr&, Buffer*, Timestamp)> MessageCallback;
typedef std::function<void (const TcpConnectionptr&)> CloseCallback;
typedef std::function<void (const TcpConnectionptr&)> WriteCompleteCallback;

typedef std::function<void()> EventCallback;
typedef std::function<void(Timestamp)> ReadEventCallback;
#endif