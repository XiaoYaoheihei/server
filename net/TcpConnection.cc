#include "TcpConnection.h"
#include <unistd.h>
#include <iostream>
#include "Channel.h"


TcpConnection::TcpConnection(EventLoop* loop, const std::string& name, int sockfd,
                const sockaddr_in& localAddr, const sockaddr_in& peerAddr)
  : loop_(loop),
    name_(name),
    state_(kConnecting),
    sockfd_(sockfd),
    channel_(new Channel(loop, sockfd)),
    localAddr_(localAddr),
    peerAddr_(peerAddr) {
      //打印日志，打印TcpConnection的相关信息

      channel_->setRead(std::bind(&TcpConnection::handleRead, this));
}

TcpConnection::~TcpConnection() = default;

EventLoop* TcpConnection::getLoop() {
  return loop_;
}
const std::string& TcpConnection::getname() {
  return name_;
}
const sockaddr_in& TcpConnection::getlocalAddr() {
  return localAddr_;
}
const sockaddr_in& TcpConnection::getpeerAddr() {
  return peerAddr_;
}

bool TcpConnection::connected() {
  return state_ == kConnected;
}

//通信fd对应的类进行的操作
void TcpConnection::connectionEstablished() {
  //打印日志

  setState(kConnected);
  //通信fd加入到poll中检测事件的发生情况
  channel_->enableReading();

}


void TcpConnection::handleRead() {
  //现在只是简单的读取消息，还没有加缓冲类
  char buf[65536];
  ssize_t n = ::read(channel_->getFd(), buf, sizeof(buf));
  //需要打印相关日志
  std::cout << "read ok" << std::endl;
}

void TcpConnection::setConnectionCallBack(const ConnectionCallback& cb) {
  connectioncallback_ = cb;
}

