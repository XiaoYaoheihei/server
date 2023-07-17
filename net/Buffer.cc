#include "Buffer.h"
#include <errno.h>
#include <sys/uio.h>
#include <unistd.h>

#include <string>

int Buffer::readableBytes() {
  return writeIndex_ - readIndex_;
}

int Buffer::writableBytes() {
  return buffer_.size() - writeIndex_;
}

int Buffer::prependableBytes() {
  return readIndex_;
}

char* Buffer::begin() {
  return &*buffer_.begin();
}

const char* Buffer::peek() {
  return begin() + readIndex_;
}

void Buffer::retrieve(size_t len) {
  if (len < readableBytes()) {
    readIndex_ += len;
  } else {
    retrieveAll();
  }
}

void Buffer::retrieveAll() {
  readIndex_ = kPrepend;
  writeIndex_ = kPrepend;
}

std::string Buffer::retrieveAllAsString() {
  return retrieveAsString(readableBytes());
}

std::string Buffer::retrieveAsString(size_t len) {
  //构造了从readIndex_开始的前len个字符
  std::string result(peek(), len);
  retrieve(len);
  return result;
}

void Buffer::append(const std::string str) {
  //通过字符数组的方式添加
  append(str.data(), str.size());
}

void Buffer::append(const char* data, size_t len) {
  //确保writable的空间中足够写
  ensureWritableBytes(len);
  std::copy(data ,data+len, beginWrite());
  hasWritten(len);
}

void Buffer::ensureWritableBytes(size_t len) {
  if (writableBytes() < len) {
    makeSpace(len);
  }
}

void Buffer::makeSpace(size_t len) {
  if (writableBytes() + prependableBytes() < len + kPrepend) {
    buffer_.resize(writeIndex_ + len);
  } else {
    //内部挪移数据
    int readable = readableBytes();
    std::copy(begin() + readIndex_, begin() + writeIndex_, begin() + kPrepend);
    readIndex_ = kPrepend;
    writeIndex_ = readIndex_ + readable;
  }
}

char* Buffer::beginWrite() {
  //获得可写的迭代器
  return begin() + writeIndex_;
}

//修改writen的范围
void Buffer::hasWritten(size_t len) {
  writeIndex_ += len;
}

void Buffer::swap(Buffer& tmp) {
  buffer_.swap(tmp.buffer_);
  std::swap(readIndex_, tmp.readIndex_);
  std::swap(writeIndex_, tmp.writeIndex_);
}

int Buffer::readFd(int fd, int* Errno) {
  //在栈上分配了一段空间
  char extrabuff[65536];
  struct iovec vec[2];
  int writeable = writableBytes();
  vec[0].iov_base = begin() + writeIndex_;
  vec[0].iov_len = writeable;
  vec[1].iov_base = extrabuff;
  vec[1].iov_len = sizeof extrabuff;

  const int iovcnt = (writeable < sizeof extrabuff) ? 2:1;
  size_t n = readv(fd, vec, iovcnt);
  //打印日志信息
  // LOG_DEBUG << n;
  if (n < 0) {
    *Errno = errno;
  } else if (n <= writeable) {
    //如果读入的数据不多，会全部读到Buffer中去
    writeIndex_ += n;
  } else {
    //如果长度超过Buffer的writeable字节数，就会读到extrabuff中
    writeIndex_ = buffer_.size();
    //然后把extrabuff中的数据append到Buffer中 
    append(extrabuff, n-writeable);
  }
  return n;
}
