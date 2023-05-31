#ifndef NET_EVENTLOOP_H
#define NET_EVENTLOOP_H

class EventLoop{
  public:
    EventLoop();
    ~EventLoop();

    void loop();

  private:
    
    bool looping;
    bool quit;

};


#endif