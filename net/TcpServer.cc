#include "TcpServer.h"
#include "Acceptor.h"
#include "EventLoop.h"
#include "EventLoopThreadPool.h"
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
    nextConnId_(0),
    poll_(new EventLoopThreadPool(loop_, name_)) {
      acceptor_->setAccpetCallback(std::bind(&TcpServer::newConnection, this, _1, _2));
}

TcpServer::~TcpServer() {

}

void TcpServer::setThreadNum(int numThread) {
  poll_->setThreadNum(numThread);
}

void TcpServer::start() {
  loop_->runInLoop(std::bind(&Acceptor::listen, acceptor_.get()));
}

void TcpServer::setConnectionCallback(const ConnectionCallback& cb) {
  connectioncallback_ = cb;
}

void TcpServer::setMessageCallback(const MessageCallback& cb) {
  messagecallback_ = cb;
}

void TcpServer::setWritecomletelyCallback(const WriteCompleteCallback& cb) {
  writecompletecallback_ = cb;
}

//为通信fd建立专门的类(可以理解成通用的客户端)进行管理
//每一个通信fd都对应一个TcpConnection
//Acceptor的回调函数handleRead会调用此函数
void TcpServer::newConnection(int sockfd, const struct sockaddr_in& peeraddr) {
  //打印日志信息

  char buf[32];
  snprintf(buf, sizeof(buf), "#%d", nextConnId_);
  ++nextConnId_;
  std::string connName = name_+buf;

  std::cout << "start to build the TcpConnection" << std::endl;
  //打印相关日志信息 

  //每次从EventLoopThreadPool中获取ioloop
  EventLoop* ioloop = poll_->getnextLoop();
  // ioloop->printThreadId();
  
  //把ioloop传给TcpConnection，保证了one loop per thread
  TcpConnectionptr conn(new TcpConnection(ioloop, connName, sockfd, localaddr_, peeraddr));
  connections_[connName] = conn;
  //TcpServer的connectioncallback给TcpConnection进行赋值
  conn->setConnectionCallBack(connectioncallback_);
  //TcpServer向TcpConnection注册closeCallback
  conn->setCloseCallback(std::bind(&TcpServer::removeConnection, this, _1));
  
  conn->setMessageCallBack(messagecallback_);
  conn->setWriteCompleteCallback(writecompletecallback_);
  conn->connectionEstablished();
}

//加线程池的部分
void TcpServer::removeConnection(const TcpConnectionptr& conn) {
  loop_->runInLoop(std::bind(&TcpServer::removeConnectionInloop, this, conn));
}

void TcpServer::removeConnectionInloop(const TcpConnectionptr& conn) {
  size_t n = connections_.erase(conn->getname());
  //打印日志

  //在自己的IOloop中进行
  EventLoop* ioloop = conn->getLoop();
  ioloop->queueInLoop(std::bind(&TcpConnection::connectionDestroyed, conn));
}