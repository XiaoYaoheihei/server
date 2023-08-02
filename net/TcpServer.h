#ifndef NET_TCPSERVER_H
#define NET_TCPSERVER_H
#include <map>
#include "EventLoop.h"
#include "TcpConnection.h"

class EventLoopThreadPool;
class Acceptor;
class TcpServer {
  public:
    TcpServer(EventLoop* loop, const sockaddr_in& addr, const std::string& name, bool reuseport);
    ~TcpServer();
    //设置线程数
    void setThreadNum(int numThread);
    void start();

    void setConnectionCallback(const ConnectionCallback& cb);
    void setMessageCallback(const MessageCallback& cb);
    void setWritecomletelyCallback(const WriteCompleteCallback& cb);

  private :
    void newConnection(int sockfd, const struct sockaddr_in& peeraddr);
    void removeConnection(const TcpConnectionptr& conn);
    void removeConnectionInloop(const TcpConnectionptr& conn);
    
    typedef std::map<std::string, TcpConnectionptr> ConnectionMap;
    EventLoop* loop_;
    const std::string name_;
    const struct sockaddr_in localaddr_;
    std::unique_ptr<Acceptor> acceptor_;
    ConnectionCallback connectioncallback_;
    //低水位回调
    WriteCompleteCallback writecompletecallback_;
    MessageCallback messagecallback_;
    ConnectionMap connections_;
    int nextConnId_;

    std::shared_ptr<EventLoopThreadPool> poll_;
};


#endif