#include "../net/poller.h"
#include <poll.h>

Poller::Poller(EventLoop* loop) 
  :owner(loop) {

} 

Poller::~Poller() {

}

void Poller::poll(ChannelList* activeChannels) {
  //阻塞循环
  int numEvents = ::poll(PollFdList.data(), PollFdList.size(), 0);
  if (numEvents > 0) {
    fillactiveChannels(numEvents, activeChannels);
  } else if (numEvents == 0) {

  } else {

  }
}


void Poller::fillactiveChannels(int numEvents, ChannelList* activeChannels) {
  for (auto event: PollFdList) {
    if (numEvents == 0) break;
    if (event.revents > 0) {
      
    }
  }
}