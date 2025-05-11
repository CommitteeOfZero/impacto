#include "workqueue.h"

#include "impacto.h"
#include <cassert>
#include <vector>
#include <utility>

#if IMPACTO_HAVE_THREADS
#include <thread>
#if __SWITCH__
#define __unix__
#endif
#include <blockingconcurrentqueue.h>
#if __SWITCH__
#undef __unix__
#endif
#endif

namespace Impacto {
namespace WorkQueue {
struct WorkItem {
  void* Data;
  WorkProc Perform;
  WorkCompletionCallbackProc OnComplete;
  void Handle();
};

static uint32_t WorkCompletedEventType = 0;

static void InitEventType() {
  assert(WorkCompletedEventType == 0);
  WorkCompletedEventType = SDL_RegisterEvents(1);
  assert(WorkCompletedEventType != 0);
}

#if IMPACTO_HAVE_THREADS

static std::vector<std::thread> ThreadPool;
static moodycamel::BlockingConcurrentQueue<WorkItem> QueueItems;
static bool StopWorkers = false;

static void WorkerThread() {
  while (true) {
    WorkItem item;
    bool hasItem =
        QueueItems.wait_dequeue_timed(item, std::chrono::milliseconds(5));
    if (StopWorkers) {
      return;
    }
    if (hasItem) {
      item.Handle();
    }
  }
}

void Init() {
  InitEventType();
  const uint32_t numThreads =
      std::min(static_cast<int>(std::thread::hardware_concurrency()), 4);
  for (uint32_t i = 0; i < numThreads; ++i) {
    ThreadPool.emplace_back(std::thread(WorkerThread));
  }
}

void Push(void* data, WorkProc worker,
          WorkCompletionCallbackProc completionCallback) {
  WorkItem item;
  item.Data = data;
  item.Perform = worker;
  item.OnComplete = completionCallback;
  if (ThreadPool.empty())
    item.Handle();
  else
    QueueItems.enqueue(std::move(item));
}
void StopWorkQueue() {
  StopWorkers = true;
  for (auto& thd : ThreadPool) {
    thd.join();
  }
  ThreadPool.clear();
}

#else

// If we don't have threads (i.e. on web), do each item right as it comes in for
// now, no actual queue involved.

void Init() { InitEventType(); }
void Push(void* data, WorkProc worker,
          WorkCompletionCallbackProc completionCallback) {
  WorkItem item;
  item.Data = data;
  item.Perform = worker;
  item.OnComplete = completionCallback;
  item.Handle();
}
void StopWorkQueue() {}

#endif

void WorkItem::Handle() {
  Perform(Data);
  WorkItem* copy = new WorkItem(*this);
  SDL_Event evt{};
  evt.type = WorkCompletedEventType;
  evt.user.data1 = copy;
  SDL_PushEvent(&evt);
}

bool HandleEvent(SDL_Event* evt) {
  if (evt->type != WorkCompletedEventType) return false;

  auto item = static_cast<WorkItem*>(evt->user.data1);
  item->OnComplete(item->Data);
  delete item;
  return true;
}

}  // namespace WorkQueue
}  // namespace Impacto