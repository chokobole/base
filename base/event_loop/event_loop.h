// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Copyright (c) 2020 The Base Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_EVENT_LOOP_EVENT_LOOP_H_
#define BASE_EVENT_LOOP_EVENT_LOOP_H_

#include <memory>

#include "base/export.h"
#include "base/thread/thread_local.h"
#include "event2/event.h"

namespace base {

class BASE_EXPORT EventLoop {
 public:
  enum Mode {
    WATCH_READ = 1 << 0,
    WATCH_WRITE = 1 << 1,
    WATCH_READ_WRITE = WATCH_READ | WATCH_WRITE
  };

  class FdWatcher {
   public:
    FdWatcher();
    virtual ~FdWatcher();

    virtual void OnFileCanRead(int Fd) = 0;
    virtual void OnFileCanWrite(int Fd) = 0;
  };

  class FdWatchController {
   public:
    FdWatchController();
    ~FdWatchController();

    bool StopWatchingFileDescriptor();

   private:
    friend class EventLoop;

    // Called by EventLoop.
    void Init(std::unique_ptr<event> e);

    // Used by EventLoop to take ownership of |event_|.
    std::unique_ptr<event> ReleaseEvent();

    void set_watcher(FdWatcher* watcher);
    void set_event_loop(EventLoop* event_loop);

    void OnFileCanRead(int Fd);
    void OnFileCanWrite(int Fd);

    std::unique_ptr<event> event_;
    EventLoop* event_loop_ = nullptr;
    FdWatcher* watcher_ = nullptr;
    // If this pointer is non-NULL, the pointee is set to true in the
    // destructor.
    bool* was_destroyed_ = nullptr;
  };

  class Delegate {
   public:
    virtual ~Delegate();

    virtual bool DoIdleWork() = 0;
  };

  EventLoop();
  ~EventLoop();
  EventLoop(const EventLoop& other) = delete;
  EventLoop& operator=(const EventLoop& other) = delete;

  static EventLoop* Current();

  void Run(Delegate* delegate);

  void Quit();

  bool WatchFileDescriptor(int Fd, bool persistent, int mode,
                           FdWatchController* controller, FdWatcher* watcher);

 private:
  // Called by libevent to tell us a registered FD can be read/written to.
  static void OnNotification(evutil_socket_t Fd, short flags, void* context);

  static ThreadLocalPointer<EventLoop>& CurrentTLS();
  static void BindToCurrentThread(EventLoop* event_loop);

  // This flag is set to false when Run should return.
  bool keep_running_;

  // This flag is set when inside Run.
  bool in_run_;

  // This flag is set if libevent has processed I/O events.
  bool processed_io_events_;

  // Libevent dispatcher.  Watches all sockets registered with it, and sends
  // readiness callbacks when a socket is ready for I/O.
  event_base* event_base_;
};

}  // namespace base

#endif  // BASE_EVENT_LOOP_EVENT_LOOP_H_
