#ifndef NET_SOCKET_H_
#define NET_SOCKET_H_

#include <arpa/inet.h>
#define MAXLISTEN 128

namespace socketopts {
  int creatSocket();
  void filladdr(struct sockaddr_in* addr, int port);
  void bindOrdie(int lfd, struct sockaddr_in* addr);
  void setListen(int lfd);
  void close(int fd);
  int acceptConn(int lfd, struct sockaddr_in *addr, socklen_t* size);
}

#endif