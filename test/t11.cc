#include "../net/TcpServer.h"
#include "../net/EventLoop.h"
#include "../net/Callbacks.h"
#include "../net/TcpConnection.h"
#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include <string>

std::string message;

void onConnection(const TcpConnectionptr& conn)
{
  if (conn->connected()) {
    printf("onConnection(): new connection [%s] from \n",
           conn->getname());
    conn->send(message);
  }
  else {
    printf("onConnection(): connection [%s] is down\n",
           conn->getname());
  }
}

void onWriteComplete(const TcpConnectionptr& conn)
{
  conn->send(message);
}

void onMessage(const TcpConnectionptr& conn,
               Buffer* buf,
               Timestamp receiveTime)
{
  printf("onMessage(): received %d bytes from connection [%s] at %s\n",
         buf->readableBytes(),
         conn->getname(),
         std::to_string(receiveTime.getms()).c_str());
  buf->retrieveAll();
}

int main() {
  printf("main(): pid = %d\n", getpid());

  std::string line;
  for (int i = 33; i < 127; ++i) {
    line.push_back(char(i));
  }
  line += line;

  for (size_t i = 0; i < 127-33; ++i) {
    message += line.substr(i, 72) + '\n';
  }

  struct sockaddr_in listenAddr;
  listenAddr.sin_family = AF_INET;
  listenAddr.sin_port = htons(9981);
  listenAddr.sin_addr.s_addr = INADDR_ANY;

  EventLoop loop;

  TcpServer server(&loop, listenAddr, "yaomou", 1);
  server.setConnectionCallback(onConnection);
  server.setMessageCallback(onMessage);
  server.setWritecomletelyCallback(onWriteComplete);
  server.start();

  loop.loop();
}