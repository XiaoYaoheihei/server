#ifndef NET_BUFFER_H
#define NET_BUFFER_H

#include <string>
#include <vector>

class Buffer {
  public:
    static const int kPrepend = 8;
    static const int kInitlialSize = 1024;

    Buffer(int initSize = kInitlialSize) 
      : buffer_(kPrepend + initSize),
        readIndex_(kPrepend),
        writeIndex_(kPrepend) {}

    void swap(Buffer& tmp);

    int readableBytes();
    int writableBytes();
    int prependableBytes();

    //通过迭代器获取数据
    char* begin();
    const char* peek();

    //恢复相关数据
    void retrieve(size_t n);
    void retrieveAll();
    std::string retrieveAllAsString();
    std::string retrieveAsString(size_t len);

    void append(const std::string str);
    void append(const char* data, size_t len);
    void ensureWritableBytes(size_t len);
    char* beginWrite();
    void hasWritten(size_t len);
    int readFd(int fd, int* Errno);
    void makeSpace(size_t len);

  private:
    std::vector<char> buffer_;
    int readIndex_;
    int writeIndex_;
};



#endif