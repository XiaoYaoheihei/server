#include <iostream>
#include "../net/Socket.h"

using namespace socketopts;

int main() {
  int lfd = creatSocket();
  struct sockaddr_in addr;
  filladdr(&addr, 10000);
  bindOrdie(lfd, &addr);
  setListen(lfd);
  struct sockaddr_in cliaddr;
  socklen_t sizeClien = sizeof(cliaddr);
  int cfd = acceptConn(lfd, &addr, &sizeClien);
  close(lfd);
  close(cfd);
  return 0;
}