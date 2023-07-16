#ifndef NET_TCPCONNECTION_H
#define NET_TCPCONNECTION_H
#include <arpa/inet.h>
#include <memory>
#include "Callbacks.h"
#include "EventLoop.h"

class TcpConnection : public std::enable_shared_from_this<TcpConnection> {
  public:
    TcpConnection(EventLoop* loop, const std::string& name, int sockfd,
                  const sockaddr_in& localAddr, const sockaddr_in& peerArrd);
    ~TcpConnection();

    EventLoop* getLoop();
    const std::string& getname();
    const sockaddr_in& getlocalAddr();
    const sockaddr_in& getpeerAddr();

    bool connected();
    void connectionEstablished();
    //当TcpServer从map中移除了此TcpConnection的时候调用此函数
    //此函数只允许被调用一次
    void connectionDestroyed();

    // void setMessageCallBack(const MessageCallback& callback);
    void setConnectionCallBack(const ConnectionCallback& callback);
    //内部只调用closecallback一次
    void setCloseCallback(const ConnectionCallback& callback);
    void setWriteCompleteCallback(const ConnectionCallback& callback);

  private:
    enum StateE {
      kConnecting,
      kConnected,
      kDisconnected,
    };

    void setState(StateE s) {state_ = s;}
    //回调事件
    void handleRead();
    void handleClose();
    void handleError();

    EventLoop* loop_;
    std::string name_;
    StateE state_;
    int sockfd_;
    std::unique_ptr<Channel> channel_;
    struct sockaddr_in localAddr_;
    struct sockaddr_in peerAddr_;

    ConnectionCallback connectioncallback_;
    // MessageCallback messagecallback_;
    CloseCallback closecallback_;
    WriteCompleteCallback writecompletecallback_;
};


#endif