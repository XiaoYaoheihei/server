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
      channel_->setWrite(std::bind(&TcpConnection::handleWrite, this));
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

//当socket变得可写的时候，Channel会调用此函数
//此时会继续发送buffer中的数据
//一旦发送完毕立即停止该事件
void TcpConnection::handleWrite() {
  if (channel_->Is_writing()) {
    ssize_t wrote = write(channel_->getFd(), outputbuf_.peek(), 
                          outputbuf_.readableBytes());
    if (wrote > 0) {
      outputbuf_.retrieve(wrote);
      if (outputbuf_.readableBytes() == 0) {
        //取消关注事件
        channel_->disableWriting();
        //触发低水位回调
        //如果缓冲区被清空，就调用该回调函数
        if (writecompletecallback_) {
          loop_->queueInLoop(std::bind(writecompletecallback_, shared_from_this()));
        }
        //如果此时连接正在关闭,调用shutdownInLloop,继续执行关闭流程
        if (state_ == kDisconnecting) {
          shutdownInloop();
        }
      }
    } else {
      //日志信息
    }
  } else {
    //日志信息
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
  std::cout << "还未进行相关处理" << std::endl;
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

//直接发送数据
void TcpConnection::send(const std::string& message) {
  if (state_ == kConnected) {
    //判断是否在IO线程里面
    if (loop_->isInloopThread()) {
      sendInloop(message);
    } else {
      loop_->runInLoop(std::bind(&TcpConnection::sendInloop, this, message));
    }
  }
}

//发送Buffer里面的数据
void TcpConnection::send(Buffer* buff) {
  if (state_ == kConnected) {
    //判断是否在IO线程里面
    if (loop_->isInloopThread()) {
      //buff里面的数据全部发送完
      sendInloop(buff->peek());
      buff->retrieveAll();
    } else {
      loop_->runInLoop(std::bind(&TcpConnection::sendInloop, this, buff->retrieveAllAsString()));
    }
  }
}

void TcpConnection::shutdown() {
  if (state_ == kConnected) {
    setState(kDisconnecting);
    //线程安全的，把实际工作放到IO线程中完成
    loop_->runInLoop(std::bind(&TcpConnection::shutdownInloop, this));
  }
}

void TcpConnection::sendInloop(const std::string& message) {
  size_t len = message.size();
  ssize_t wrote = 0;//已发送的消息
  size_t remaining = message.size();
  //连接已经关闭了
  if (state_ == kDisconnected) {
    // LOG_WARN << "disconnected";
    return;
  }
  std::cout << len << std::endl;
  //打印日志，表明此时连接没有关闭，是可以发送message的
  // LOG_DEBUG << message;
  //此时outputbuff中没有消息，直接发送
  //如果当前的outputbuff中已经有待发送的数据，就不能发送数据了
  if (!channel_->Is_writing() && outputbuf_.readableBytes() == 0) {
    std::cout << "start to write" << std::endl;
    //会尝试直接先发送数据
    wrote = ::write(channel_->getFd(), message.data(), len);
    if (wrote >= 0) {
      remaining = len - wrote;
      std::cout << remaining << std::endl;
      if (remaining == 0 && writecompletecallback_) {
        //触发低水位回调
        //已经将数据从用户缓冲区全部转移到内核缓冲区
        loop_->queueInLoop(std::bind(writecompletecallback_, shared_from_this()));
      }
    } else {
      wrote = 0;
      //出现了某种错误导致write系统调用发生失败
    }
  } 
  //如果只是发送了部分数据，则把剩余的数据追加到outputbuff中，
  //并且开始关注writeable事件，以后在handlewrite中发送剩余的数据
  if (remaining != 0) {
    outputbuf_.append(message.data() + wrote, message.size() - wrote);
    if (!channel_->Is_writing()) {
      channel_->enableWriting();
    }
  }
}

void TcpConnection::shutdownInloop() {
  //必须保证此时没有正在写
  if (!channel_->Is_writing()) {
    std::cout << "将此fd shutdown" << std::endl;
    //关闭发包
    ::shutdown(sockfd_, SHUT_WR);
  }
}

// void TcpConnection::setTcpNoDelay(bool on) {
//   int optval = on ? 1 :0;
//   ::setsockopt(sockfd_, IPPROTO_TCP, TCP_NODELAY)
// }