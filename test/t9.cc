#include "../net/TcpServer.h"
#include "../net/EventLoop.h"
#include "../net/Callbacks.h"
#include "../net/TcpConnection.h"
#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include <string>

void onConnection(const TcpConnectionptr& conn)
{
  if (conn->connected()) {
    printf("onConnection(): new connection [%s] from \n",
           conn->getname());
  }
  else {
    printf("onConnection(): connection [%s] is down\n",
           conn->getname());
  }
}


void onMessage(const TcpConnectionptr& conn,
               Buffer* buf,
               Timestamp receiveTime)
{
  printf("onMessage(): received %d bytes from connection [%s] at %s\n",
         buf->readableBytes(),
         conn->getname(),
         std::to_string(receiveTime.getms()).c_str());

  conn->send(buf->retrieveAllAsString());
}

int main()
{
  printf("main(): pid = %d\n", getpid());

  struct sockaddr_in listenAddr;
  listenAddr.sin_family = AF_INET;
  listenAddr.sin_port = htons(9981);
  listenAddr.sin_addr.s_addr = INADDR_ANY;

  EventLoop loop;

  TcpServer server(&loop, listenAddr, "yaomou", 1);
  server.setConnectionCallback(onConnection);
  server.setMessageCallback(onMessage);
  server.start();

  loop.loop();
}