#include "../net/Acceptor.h"
#include "../net/EventLoop.h"
#include "../net/Socket.h"
#include "../net/Callbacks.h"
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
//测试Acceptor功能,需要添加一个客户端进行模拟

void newConnection(int sockfd, const struct sockaddr_in& peerAddr) {
//发送一个字符串之后随机断开连接
    printf("newConnection(): accepted a new connection from \n");
    ::write(sockfd, "How are you?\n", 13);
    printf("send success\n");
    socketopts::close(sockfd);
}

int main() {
    printf("main(): pid = %d\n", getpid());
    //初始化本机地址
    //通过端口号进行初始化，在9981端口进行监听，连接到达之后调用用户回调
    struct sockaddr_in listenAddr;
    listenAddr.sin_family = AF_INET;
    listenAddr.sin_port = htons(9981);
    listenAddr.sin_addr.s_addr = INADDR_ANY;
    EventLoop loop;
    //初始化listenfd
    Acceptor acceptor(&loop, listenAddr, 1);
    acceptor.setAccpetCallback(newConnection);
    acceptor.listen();

    loop.loop();
}