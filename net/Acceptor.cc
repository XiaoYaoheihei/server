#include "Acceptor.h"
#include "Socket.h"
#include "EventLoop.h"

Acceptor::Acceptor(EventLoop* loop, 
                   const struct sockaddr_in& listenAddr,
                   bool reuseport) 
  : socketfd_(socketopts::creatSocket()),
    sockaddr_(listenAddr),
    loop_(loop),
    acceptChannel_(loop, socketfd_),
    listening_(false) {
      socketopts::setReuseport(socketfd_, reuseport);
      socketopts::bindOrdie(socketfd_, &sockaddr_);
      acceptChannel_.setRead(std::bind(&Acceptor::handleRead, this));
}

Acceptor::~Acceptor() {

}

//设置回调函数
void Acceptor::setAccpetCallback(const NewConnectionCallback& cb) {
  acceptcb_ = std::move(cb);
}

void Acceptor::listen() {
  listening_ = true;
  socketopts::setListen(socketfd_);
  //添加fd到poll中去
  acceptChannel_.enableReading();
}

bool Acceptor::listening() {
  return listening_;
}

void Acceptor::handleRead() {
  int cfd;
  struct sockaddr_in caddr;
  cfd = socketopts::acceptConn(socketfd_, &sockaddr_);
  //打印日志，打印通信fd

  if (cfd > 0) {
    //调用回调函数
    acceptcb_(cfd, caddr);
  } else {
    socketopts::close(cfd);
  }
}