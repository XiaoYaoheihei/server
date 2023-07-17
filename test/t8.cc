#include "../net/TcpServer.h"
#include "../net/TcpConnection.h"
#include "../net/EventLoop.h"
#include "../net/Callbacks.h"
#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <string>
//测试完整地建立一次连接,只不过此时客户端退出的时候服务端fd没有从Poll中移除

void onConnection(const TcpConnectionptr& conn) {
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
  printf("onMessage(): received %zd bytes from connection [%s] at %s\n",
         buf->readableBytes(),
         conn->getname(),
         std::to_string(receiveTime.getms()).c_str());
        //  receiveTime.toFormattedString().c_str());

  // printf("onMessage(): [%s]\n", buf->retrieveAsString().c_str());
}

int main() {
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
  printf("设置完成\n");
  loop.loop();
}