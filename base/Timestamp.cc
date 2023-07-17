#include "Timestamp.h"
#include <sys/time.h>

Timestamp::Timestamp() 
  : microSecondsSinceEpoch(0) {
}

Timestamp::Timestamp(int64_t microSecond) 
  : microSecondsSinceEpoch(microSecond) {
}

//获取当前的时间戳
Timestamp Timestamp::now() {
  struct timeval tv;
  //首先获取当前的时间戳
  gettimeofday(&tv, NULL);
  //得到具体多少秒
  int64_t seconds = tv.tv_sec;
  //计算出的微秒+微秒部分
  return Timestamp(seconds*kMicroSecondsPerSecond + tv.tv_usec);
}