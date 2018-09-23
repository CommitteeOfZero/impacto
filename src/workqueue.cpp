#include <deque>

#include "workqueue.h"

#include "impacto.h"

namespace Impacto {

struct WorkItem {
  void* Data;
  WorkProc Perform;
  WorkCompletionCallbackProc OnComplete;
};

static int WorkCompletedEventType = -1;
static std::deque<WorkItem> WorkQueue;
static SDL_mutex* Lock;
static SDL_cond* WorkSubmitted;

static int WorkerThread(void* unused) {
  for (;;) {
    SDL_LockMutex(Lock);
    if (WorkQueue.empty()) {
      SDL_CondWait(WorkSubmitted, Lock);
      SDL_UnlockMutex(Lock);
    } else {
      WorkItem item = WorkQueue.front();
      WorkQueue.pop_front();
      SDL_UnlockMutex(Lock);
      item.Perform(item.Data);

      SDL_Event evt;
      memset(&evt, 0, sizeof(evt));
      evt.type = WorkCompletedEventType;
      evt.user.data1 = item.Data;
      evt.user.data2 = item.OnComplete;
      SDL_PushEvent(&evt);
    }
  }
}

void WorkQueueInit() {
  assert(WorkCompletedEventType == -1);
  WorkCompletedEventType = SDL_RegisterEvents(1);
  assert(WorkCompletedEventType != -1);
  Lock = SDL_CreateMutex();
  WorkSubmitted = SDL_CreateCond();
  SDL_CreateThread(&WorkerThread, "Worker thread", NULL);
}

void WorkQueuePush(void* data, WorkProc worker,
                   WorkCompletionCallbackProc completionCallback) {
  SDL_LockMutex(Lock);
  WorkItem item;
  item.Data = data;
  item.Perform = worker;
  item.OnComplete = completionCallback;
  WorkQueue.push_back(item);
  SDL_CondSignal(WorkSubmitted);
  SDL_UnlockMutex(Lock);
}

bool WorkQueueHandleEvent(SDL_Event* evt) {
  if (evt->type != WorkCompletedEventType) return false;

  void* Data = evt->user.data1;
  WorkCompletionCallbackProc Callback =
      (WorkCompletionCallbackProc)evt->user.data2;
  Callback(Data);
  return true;
}

}  // namespace Impacto