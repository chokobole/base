// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Copyright (c) 2020 The Base Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_TASK_RUNNER_H_
#define BASE_TASK_RUNNER_H_

#include <stddef.h>

#include "base/export.h"
#include "base/callback.h"
#include "absl/time/time.h"

namespace base {

struct TaskRunnerTraits;

// A TaskRunner is an object that runs posted tasks (in the form of
// OnceClosure objects).  The TaskRunner interface provides a way of
// decoupling task posting from the mechanics of how each task will be
// run.  TaskRunner provides very weak guarantees as to how posted
// tasks are run (or if they're run at all).  In particular, it only
// guarantees:
//
//   - Posting a task will not run it synchronously.  That is, no
//     Post*Task method will call task.Run() directly.
//
//   - Increasing the delay can only delay when the task gets run.
//     That is, increasing the delay may not affect when the task gets
//     run, or it could make it run later than it normally would, but
//     it won't make it run earlier than it normally would.
//
// TaskRunner does not guarantee the order in which posted tasks are
// run, whether tasks overlap, or whether they're run on a particular
// thread.  Also it does not guarantee a memory model for shared data
// between tasks.  (In other words, you should use your own
// synchronization/locking primitives if you need to share data
// between tasks.)
//
// Implementations of TaskRunner should be thread-safe in that all
// methods must be safe to call on any thread.
//
// Some theoretical implementations of TaskRunner:
//
//   - A TaskRunner that uses a thread pool to run posted tasks.
//
//   - A TaskRunner that, for each task, spawns a non-joinable thread
//     to run that task and immediately quit.
//
//   - A TaskRunner that stores the list of posted tasks and has a
//     method Run() that runs each runnable task in random order.
class BASE_EXPORT TaskRunner {
 public:
  TaskRunner();
  virtual ~TaskRunner();

  // Posts the given task to be run.  Returns true if the task may be
  // run at some point in the future, and false if the task definitely
  // will not be run.
  //
  // Equivalent to PostDelayedTask(task, 0).
  bool PostTask(OnceClosure task);

  // Like PostTask, but tries to run the posted task only after |delay_ms|
  // has passed. Implementations should use a tick clock, rather than wall-
  // clock time, to implement |delay|.
  virtual bool PostDelayedTask(OnceClosure task,
                               absl::Duration delay) = 0;

  // Posts |task| on the current TaskRunner.  On completion, |reply|
  // is posted to the thread that called PostTaskAndReply().  Both
  // |task| and |reply| are guaranteed to be deleted on the thread
  // from which PostTaskAndReply() is invoked.  This allows objects
  // that must be deleted on the originating thread to be bound into
  // the |task| and |reply| OnceClosures.
  bool PostTaskAndReply(OnceClosure task,
                        OnceClosure reply);

 protected:
  friend struct TaskRunnerTraits;

  // Called when this object should be destroyed.  By default simply
  // deletes |this|, but can be overridden to do something else, like
  // delete on a certain thread.
  virtual void OnDestruct() const;
};

struct BASE_EXPORT TaskRunnerTraits {
  static void Destruct(const TaskRunner* task_runner);
};

}  // namespace base

#endif  // BASE_TASK_RUNNER_H_
