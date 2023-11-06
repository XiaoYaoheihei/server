#include <iostream>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<arpa/inet.h>
#include<sys/stat.h>
#include<fcntl.h>
#include "./net/Socket.h"


int main() {
  int fd = socketopts::creatSocket();
  if (fd == -1) {
    std::cout << "fail to create" << std::endl;
    return -1;
  }

  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(9981);   //端口转换成大端端口
  inet_pton(AF_INET, "172.19.0.1", &addr.sin_addr.s_addr);
    //ip地址转化成大端

  int ret = connect(fd, (struct sockaddr*)&addr, sizeof(addr));
  if (ret == -1) {
      perror("connect");
      return -1;
        //exit(0);
  }
    std::cout << "success to connnect" << std::endl;
    // sleep(3);
    std::cout << "start to send message" << std::endl;
    char buf[512] = "you guess if success or not";
    write(fd, buf, sizeof buf);
    // sleep(5);
    int readn = 0;
    while (1) {
      
      int n = read(fd, buf, sizeof buf);
      if (n > 0) {
        std::cout << std::string(buf) << std::endl;
        readn += n;
      }
      if (readn >= 512) {
        break;
      }
    }
    return 0;
}