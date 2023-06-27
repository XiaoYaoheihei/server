#include "TcpServer.h"
#include "Acceptor.h"
#include "EventLoop.h"
#include <iostream>

//这个存在是干什么的
using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;

TcpServer::TcpServer(EventLoop* loop, const sockaddr_in& addr,
                     const std::string& name, bool reuseport)
  : loop_(loop),
    name_(name),
    localaddr_(addr),
    acceptor_(new Acceptor(loop, addr, reuseport)),
    nextConnId_(0) {
      acceptor_->setAccpetCallback(std::bind(&TcpServer::newConnection, this, _1, _2));
}

TcpServer::~TcpServer() {

}

void TcpServer::start() {
  loop_->runInLoop(std::bind(&Acceptor::listen, acceptor_.get()));
}

void TcpServer::setConnectionCallback(const ConnectionCallback& cb) {
  connectioncallback_ = cb;
}

//为通信fd建立专门的类(可以理解成通用的客户端)进行管理
//每一个通信fd都对应一个TcpConnection
//Acceptor的回调函数handleRead会调用此函数
void TcpServer::newConnection(int sockfd, const struct sockaddr_in& peeraddr) {
  char buf[32];
  snprintf(buf, sizeof(buf), "#%d", nextConnId_);
  ++nextConnId_;
  std::string connName = name_+buf;

  std::cout << "start to build the TcpConnection" << std::endl;
  //打印相关日志信息 

  TcpConnectionptr conn(new TcpConnection(loop_, connName, sockfd, localaddr_, peeraddr));
  connections_[connName] = conn;
  conn->setConnectionCallBack(connectioncallback_);

  conn->connectionEstablished();
}