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


    // void setMessageCallBack(const MessageCallback& callback);
    void setConnectionCallBack(const ConnectionCallback& callback);
  
  private:
    enum StateE {
      kConnecting,
      kConnected,
    };

    void setState(StateE s) {state_ = s;}
    //回调事件
    void handleRead();

    EventLoop* loop_;
    std::string name_;
    StateE state_;
    int sockfd_;
    std::unique_ptr<Channel> channel_;
    struct sockaddr_in localAddr_;
    struct sockaddr_in peerAddr_;

    ConnectionCallback connectioncallback_;
    // MessageCallback messagecallback_;

};


#endif