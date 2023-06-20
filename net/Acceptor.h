#ifndef NET_ACCEPTOR_H
#define NET_ACCEPTOR_H
#include <arpa/inet.h>
#include "Callbacks.h"
#include "Channel.h"

class Acceptor{
  public:
    Acceptor(EventLoop* loop, const struct sockaddr_in& listenAddr, bool reuseport);
    ~Acceptor();

    void setAccpetCallback(const NewConnectionCallback& cb);
    void listen();
    bool listening();

  private:
    void handleRead();

    int socketfd_;
    const struct sockaddr_in sockaddr_;
    EventLoop* loop_;
    Channel acceptChannel_;
    bool listening_;
    NewConnectionCallback acceptcb_;
    
};

#endif