# server
### c++ server实现：

- 基于事件驱动的reactor模型
- 采用基于one loop per thread+thread pool并发模型
- 采用Linux下的epoll IO复用机制
- 异步回调机制对事件进行分发和处理
- 基于eventfd的无锁事件分发
- 封装用户态Buffer类，为每一个连接维护Buffer类实现数据发送与接受
- 最小堆+timerfd实现定时器功能
- 采用RAII管理内存，避免内存泄漏

### 需要改进的：

- 实现基于前后端分离，多缓冲区的高效多线程异步日志库（正在进行中√）
- 在用户层实现HTTP协议，支持静态资源的访问，采用状态机对HTTP协议进行解析
- 后续还需要进行一些性能分析和优化
- 加入新的c++版本上的一些特性
