#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include <cerrno>
#include "Socket.h"


int socketopts::creatSocket() {
  int fd =::socket(AF_INET, SOCK_STREAM, 0);
  //检查返回值
  if (fd == -1) {
    perror("socket");
    exit(0);
  }
  //添加日志信息
  printf("success to creat fd\n");
  return fd;
}

void socketopts::filladdr(struct sockaddr_in* addr, int port) {
  memset(addr, 0, sizeof(struct sockaddr_in));
  addr->sin_family = AF_INET;
  addr->sin_port = htons(port);
  addr->sin_addr.s_addr = INADDR_ANY;
}

void socketopts::setReuseport(int sockfd, bool on) {
  int options = on ? 1 : 0;
  //设置端口复用
  int ret = ::setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &options,
                        static_cast<socklen_t>(sizeof options));
  if (ret) {

  }
}

void socketopts::bindOrdie(int lfd, const struct sockaddr_in* addr) {
  int ret = ::bind(lfd, (struct sockaddr*)addr, sizeof(*addr));
  if (ret == -1) {
    perror("bind");
    exit(0);
  }
  //添加日志信息
  printf("success to bind\n");
}

void socketopts::setListen(int lfd) {
  int ret = ::listen(lfd, MAXLISTEN);
  if (ret == -1) {
    perror("listrn");
    exit(0);
  }
  //添加日志信息
  printf("success to listen\n");
}

int socketopts::acceptConn(int lfd,const struct sockaddr_in *listenAddr) {
  int cfd;
  socklen_t clen;
  struct sockaddr_in caddr;
  
  cfd = ::accept4(lfd, (struct sockaddr*)&caddr, &clen, 
                  SOCK_CLOEXEC | SOCK_NONBLOCK);
  if (cfd < 0) {
    int errornum = errno;
    switch (errornum)
    {
    case EBADF:
      break;
    
    case EFAULT:

    default:
      break;
    }
  }
  //添加日志信息
  printf("success to accept\n cfd == %d\n", cfd);
  return cfd;
}

void socketopts::close(int socketfd) {
  ::close(socketfd);
  printf("close the fd");
}