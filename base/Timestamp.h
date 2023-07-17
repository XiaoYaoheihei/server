#ifndef BASE_TIMESTAMP_H
#define BASE_TIMESTAMP_H

#include <iostream>
class Timestamp {
  public:
    Timestamp();
    Timestamp(int64_t microSecond);
    static Timestamp now();
    int64_t getms() {
      return microSecondsSinceEpoch;
    }
    //一秒对应多少微秒
    static const int kMicroSecondsPerSecond = 1000 * 1000;
  private:
    int64_t microSecondsSinceEpoch;

};


#endif