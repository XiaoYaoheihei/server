#ifndef NET_TCPSERVER_H
#define NET_TCPSERVER_H
#include <map>
#include "EventLoop.h"
#include "TcpConnection.h"

class Acceptor;
class TcpServer {
  public:
    TcpServer(EventLoop* loop, const sockaddr_in& addr, const std::string& name, bool reuseport);
    ~TcpServer();

    void start();

    void setConnectionCallback(const ConnectionCallback& cb);
    // void setMessageCallback(const MessageCallback& cb);
    
  private:
    void newConnection(int sockfd, const struct sockaddr_in& peeraddr);
    void removeConnection(const TcpConnectionptr& conn);

    typedef std::map<std::string, TcpConnectionptr> ConnectionMap;
    EventLoop* loop_;
    const std::string name_;
    const struct sockaddr_in localaddr_;
    std::unique_ptr<Acceptor> acceptor_;
    ConnectionCallback connectioncallback_;
    // MessageCallback messagecallback_;
    ConnectionMap connections_;
    int nextConnId_;
};


#endif