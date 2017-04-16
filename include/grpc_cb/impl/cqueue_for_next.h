// Complete queue for next.

#ifndef GRPC_CB_IMPL_CQUEUE_FOR_NEXT_H
#define GRPC_CB_IMPL_CQUEUE_FOR_NEXT_H

#include <grpc_cb/impl/completion_queue.h>  // for CompletionQueue

namespace grpc_cb {

/// A thin wrapper around grpc_completion_queue for next.
/// Thread-safe.
class CQueueForNext : public CompletionQueue {
 public:
  /// Default constructor. Implicitly creates a \a grpc_completion_queue
  /// instance.
  CQueueForNext();

  /// Wrap \a take, taking ownership of the instance.
  ///
  /// \param take The completion queue instance to wrap. Ownership is taken.
  explicit CQueueForNext(grpc_completion_queue* take);

  /// Destructor. Destroys the owned wrapped completion queue instance.
  virtual ~CQueueForNext() GRPC_OVERRIDE;

 public:
  /// Wraps \a grpc_completion_queue_next.
  /// \param deadline[in] How long to block in wait for an event.
  template <typename T>
  grpc_event Next(const T& deadline) {
    TimePoint<T> deadline_tp(deadline);
    return NextInternal(deadline_tp.raw_time());
  }

  grpc_event Next() {
    return NextInternal(gpr_inf_future(GPR_CLOCK_REALTIME));
  }
  grpc_event TryNext() {
    return NextInternal(gpr_time_0(GPR_CLOCK_REALTIME));
  }

 private:
  grpc_event NextInternal(gpr_timespec deadline);
};

}  // namespace grpc_cb

#endif  // GRPC_CB_IMPL_CQUEUE_FOR_NEXT_H
