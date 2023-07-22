#include "../net/TcpServer.h"
#include "../net/EventLoop.h"
#include "../net/Callbacks.h"
#include "../net/TcpConnection.h"
#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include <string>

std::string message1;
std::string message2;
int sleepSeconds = 0;

void onConnection(const TcpConnectionptr& conn)
{
  if (conn->connected()) {
    printf("onConnection(): new connection [%s] from \n",
           conn->getname());
    if (sleepSeconds > 0) {
      ::sleep(sleepSeconds);
    }
    conn->send(message1);
    conn->send(message2);
    conn->shutdown();
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
  buf->retrieveAll();
}

int main() {
  printf("main(): pid = %d\n", getpid());

  int len1 = 100;
  int len2 = 200;

  message1.resize(len1);
  message2.resize(len2);
  std::fill(message1.begin(), message1.end(), 'A');
  std::fill(message2.begin(), message2.end(), 'B');

  struct sockaddr_in listenAddr;
  listenAddr.sin_family = AF_INET;
  listenAddr.sin_port = htons(9981);
  listenAddr.sin_addr.s_addr = INADDR_ANY;

  EventLoop loop;

  TcpServer server(&loop, listenAddr, "yaomou", 1);
  server.setConnectionCallback(onConnection);
  server.setMessageCallback(onMessage);
  // server.setWritecomletelyCallback(onWriteComplete);
  server.start();

  loop.loop();
}