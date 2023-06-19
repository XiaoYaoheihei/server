#include "Thread.h"
#include <unistd.h>
#include <syscall.h>

Thread::Thread(ThreadFunc func)
  : started_(false),
    tid_(0),
    func_(std::move(func)){}

Thread::~Thread() { }

//我在想这个ThreadData是否可以放在Thread的private成员里面
//保存线程的相关数据
class ThreadData {
  public:
    typedef std::function<void()> ThreadFunc;
    ThreadData(ThreadFunc& func, std::string& name, pid_t* tid,
              std::promise<void>& latch)
        : func_(func), name_(name), tid_(tid), latch_(latch){};
    ~ThreadData() = default;

    ThreadFunc func_;
    std::string name_;
    pid_t* tid_;
    //通过引用进行修改
    std::promise<void>& latch_;
};

pid_t gettid() {
  return syscall(SYS_gettid);
}

void *runInThread(void* arg) {
  //类型转化运算，把void*转化成ThreadDate*类型
  ThreadData* data = static_cast<ThreadData*>(arg);
  *data->tid_ = gettid();
  data->latch_.set_value();
  //此时才执行创建EventLoop对象
  data->func_();

  return NULL;
}


void Thread::start() {
  //首先获得future对象，用于异步获取变量值
  std::future<void> latch = latch_.get_future();
  started_ = true;
  ThreadData* data = new ThreadData(func_, name_, &tid_, latch_);
  //再新建一个线程进行处理
  if (pthread_create(&pthreadId_, NULL, &runInThread, data)) {
    started_ = false;
    delete data;
    //打印相关日志显示创建线程失败

  } else {
    latch.get();
  }
}

int Thread::join() {
  return pthread_join(pthreadId_, NULL);
}