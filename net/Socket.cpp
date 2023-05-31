#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
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
  addr->sin_family = AF_INET;
  addr->sin_port = htons(port);
  addr->sin_addr.s_addr = INADDR_ANY;
}

void socketopts::bindOrdie(int lfd, struct sockaddr_in* addr) {
  int options = 1;
  //设置端口复用
  int ret = ::setsockopt(lfd, SOL_SOCKET, SO_REUSEPORT, &options,
                        static_cast<socklen_t>(sizeof options));
  if (ret) {

  }
  ret = ::bind(lfd, (struct sockaddr*)addr, sizeof(*addr));
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

int socketopts::acceptConn(int lfd, struct sockaddr_in *listenAddr, socklen_t* size) {
  int cfd = ::accept(lfd, (struct sockaddr*)listenAddr, size);
  if (cfd == -1) {
    perror("accept");
    exit(0);
  }
  //添加日志信息
  printf("success to accept\n");
  return cfd;
}

void socketopts::close(int socketfd) {
  close(socketfd);
  printf("close the fd");
}