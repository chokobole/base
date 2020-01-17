// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Copyright (c) 2020 The Base Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/event_loop/event_loop.h"

#include "base/auto_reset.h"
#include "base/logging.h"
#include "base/no_destructor.h"
#include "event2/event_compat.h"
#include "event2/event_struct.h"

namespace base {

EventLoop::FdWatcher::FdWatcher() = default;

EventLoop::FdWatcher::~FdWatcher() = default;

EventLoop::FdWatchController::FdWatchController() = default;

EventLoop::FdWatchController::~FdWatchController() {
  if (event_) {
    CHECK(StopWatchingFileDescriptor());
  }
  if (was_destroyed_) {
    DCHECK(!*was_destroyed_);
    *was_destroyed_ = true;
  }
}

bool EventLoop::FdWatchController::StopWatchingFileDescriptor() {
  std::unique_ptr<event> e = ReleaseEvent();
  if (!e) return true;

  // event_del() is a no-op if the event isn't active.
  int rv = event_del(e.get());
  event_loop_ = nullptr;
  watcher_ = nullptr;
  return (rv == 0);
}

void EventLoop::FdWatchController::Init(std::unique_ptr<event> e) {
  DCHECK(e);
  DCHECK(!event_);

  event_ = std::move(e);
}

std::unique_ptr<event> EventLoop::FdWatchController::ReleaseEvent() {
  return std::move(event_);
}

void EventLoop::FdWatchController::set_watcher(FdWatcher* watcher) {
  watcher_ = watcher;
}

void EventLoop::FdWatchController::set_event_loop(EventLoop* event_loop) {
  event_loop_ = event_loop;
}

void EventLoop::FdWatchController::OnFileCanRead(int fd) {
  watcher_->OnFileCanRead(fd);
}

void EventLoop::FdWatchController::OnFileCanWrite(int fd) {
  watcher_->OnFileCanWrite(fd);
}

EventLoop::Delegate::~Delegate() = default;

EventLoop::EventLoop() : event_base_(event_base_new()) {
  CHECK(event_base_);
  BindToCurrentThread(this);
}

EventLoop::~EventLoop() {
  DCHECK(event_base_);
  event_base_free(event_base_);
  BindToCurrentThread(nullptr);
}

// static
EventLoop* EventLoop::Current() { return CurrentTLS().Get(); }

void EventLoop::Run(Delegate* delegate) {
  AutoReset<bool> auto_reset_keep_running(&keep_running_, true);
  AutoReset<bool> auto_reset_in_run(&in_run_, true);

  for (;;) {
    if (!keep_running_) break;

    event_base_loop(event_base_, EVLOOP_NONBLOCK);
    bool more_work_is_plausible = processed_io_events_;
    processed_io_events_ = false;

    if (!keep_running_) break;

    if (more_work_is_plausible) continue;

    more_work_is_plausible = delegate->DoIdleWork();

    if (more_work_is_plausible) continue;

    event_base_loop(event_base_, EVLOOP_ONCE);

    if (!keep_running_) break;
  }
}

void EventLoop::Quit() {
  event_base_loopexit(event_base_, nullptr);
  keep_running_ = false;
}

bool EventLoop::WatchFileDescriptor(int fd, bool persistent, int mode,
                                    FdWatchController* controller,
                                    FdWatcher* watcher) {
  DCHECK_GE(fd, 0);
  DCHECK(controller);
  DCHECK(watcher);
  DCHECK(mode == WATCH_READ || mode == WATCH_WRITE || mode == WATCH_READ_WRITE);

  int event_mask = persistent ? EV_PERSIST : 0;
  if (mode & WATCH_READ) {
    event_mask |= EV_READ;
  }
  if (mode & WATCH_WRITE) {
    event_mask |= EV_WRITE;
  }

  std::unique_ptr<event> evt(controller->ReleaseEvent());
  if (!evt) {
    // Ownership is transferred to the controller.
    evt.reset(new event);
  } else {
    // Make sure we don't pick up any funky internal libevent masks.
    int old_interest_mask = evt->ev_events & (EV_READ | EV_WRITE | EV_PERSIST);

    // Combine old/new event masks.
    event_mask |= old_interest_mask;

    // Must disarm the event before we can reuse it.
    event_del(evt.get());

    // It's illegal to use this function to listen on 2 separate fds with the
    // same |controller|.
    if (event_get_fd(evt.get()) != fd) {
      NOTREACHED() << "FDs don't match" << event_get_fd(evt.get())
                   << "!=" << fd;
      return false;
    }
  }

  // Set current interest mask and message pump for this event.
  event_set(evt.get(), fd, event_mask, OnNotification, controller);

  // Tell libevent which message pump this socket will belong to when we add it.
  if (event_base_set(event_base_, evt.get())) {
    DPLOG(ERROR) << "event_base_set(fd=" << event_get_fd(evt.get()) << ")";
    return false;
  }

  // Add this socket to the list of monitored sockets.
  if (event_add(evt.get(), nullptr)) {
    DPLOG(ERROR) << "event_add failed(fd=" << event_get_fd(evt.get()) << ")";
    return false;
  }

  controller->Init(std::move(evt));
  controller->set_watcher(watcher);
  controller->set_event_loop(this);
  return true;
}

// static
void EventLoop::OnNotification(evutil_socket_t fd, short flags, void* context) {
  FdWatchController* controller = static_cast<FdWatchController*>(context);
  DCHECK(controller);

  controller->event_loop_->processed_io_events_ = true;

  if ((flags & (EV_READ | EV_WRITE)) == (EV_READ | EV_WRITE)) {
    // Both callbacks will be called. It is necessary to check that |controller|
    // is not destroyed.
    bool controller_was_destroyed = false;
    controller->was_destroyed_ = &controller_was_destroyed;
    controller->OnFileCanWrite(fd);
    if (!controller_was_destroyed) controller->OnFileCanRead(fd);
    if (!controller_was_destroyed) controller->was_destroyed_ = nullptr;
  } else if (flags & EV_WRITE) {
    controller->OnFileCanWrite(fd);
  } else if (flags & EV_READ) {
    controller->OnFileCanRead(fd);
  }
}

// static
ThreadLocalPointer<EventLoop>& EventLoop::CurrentTLS() {
  static NoDestructor<ThreadLocalPointer<EventLoop>> event_loop_tls;
  return *event_loop_tls;
}

// static
void EventLoop::BindToCurrentThread(EventLoop* event_loop) {
  CurrentTLS().Set(event_loop);
}

}  // namespace base