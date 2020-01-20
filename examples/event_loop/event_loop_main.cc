#include <iostream>

#include "base/event_loop/event_loop.h"

class Worker : public base::EventLoop::Delegate {
 public:
  Worker(base::EventLoop* event_loop, int count)
      : event_loop_(event_loop), count_(count) {}

  bool DoIdleWork() override {
    if (count_ == 0) {
      event_loop_->Quit();
      return false;
    }
    std::cout << "DoIdleWork" << std::endl;
    count_--;
    return true;
  }

 private:
  base::EventLoop* event_loop_;
  int count_;
};

int main(int argc, char** argv) {
  base::EventLoop event_loop;
  Worker* worker = new Worker(&event_loop, 5);
  event_loop.Run(worker);

  return 0;
}