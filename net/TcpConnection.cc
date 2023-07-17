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

      channel_->setRead(std::bind(&TcpConnection::handleRead, this, std::placeholders::_1));
      channel_->setClose(std::bind(&TcpConnection::handleClose, this));
      channel_->setError(std::bind(&TcpConnection::handleError, this));
      // channel_->setWriteCallBack(std::bind(&TcpConnection::handleWrite, this));
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
  //调用回调函数
  connectioncallback_(shared_from_this());
}

void TcpConnection::connectionDestroyed() {
  if (state_ == kConnected) {
    setState(kDisconnected);
    channel_->disableAll();
    connectioncallback_(shared_from_this());
  }
  //移除channel
  channel_->remove();
}


void TcpConnection::handleRead(Timestamp receiveTime) {
  //现在只是简单的读取消息，还没有加缓冲类
  // char buf[65536];
  // ssize_t n = ::read(channel_->getFd(), buf, sizeof(buf));
  int Errno;
  int n = inputbuf_.readFd(channel_->getFd(), &Errno);
  //需要打印相关日志
  std::cout << "read ok" << std::endl;
  if (n > 0) {
    //要进行相关通信
    //后续需要添加messagecallback
    // LOG_DEBUG << n;
    std::cout << "read message is:" << n << std::endl;
    messagecallback_(shared_from_this(), &inputbuf_, receiveTime);
  } else if (n == 0) {
    std::cout << "need to close" << std::endl;
    //如果是对面客户端主动退出，channel中读取的是0!!!
    handleClose();
  } else {
    handleError();
  }
}

//主要功能是调用closeCallback,然后把这个回调绑定到
//TcpServer::removeConnection()中
void TcpConnection::handleClose() {
  setState(kDisconnected);
  //移除相关channel
  channel_->disableAll();
  //这里的shared_from_this是什么用法
  TcpConnectionptr This(shared_from_this());
  closecallback_(This);
}

void TcpConnection::handleError() {
  //只是在日志中输出错误信息，并不会影响连接的正常关闭

}

void TcpConnection::setConnectionCallBack(const ConnectionCallback& cb) {
  connectioncallback_ = cb;
}

void TcpConnection::setCloseCallback(const ConnectionCallback& cb) {
  closecallback_ = cb;
}

void TcpConnection::setWriteCompleteCallback(const ConnectionCallback& cb) {
  writecompletecallback_ = cb;
}

void TcpConnection::setMessageCallBack(const MessageCallback& cb) {
  messagecallback_ = cb;
}