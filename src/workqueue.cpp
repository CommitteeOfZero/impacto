#include <deque>

#include "workqueue.h"

#include "impacto.h"

namespace Impacto {
namespace WorkQueue {
struct WorkItem {
  void* Data;
  WorkProc Perform;
  WorkCompletionCallbackProc OnComplete;

  void Handle();
};

static int WorkCompletedEventType = -1;

static void InitEventType() {
  assert(WorkCompletedEventType == -1);
  WorkCompletedEventType = SDL_RegisterEvents(1);
  assert(WorkCompletedEventType != -1);
}

#ifdef IMPACTO_HAVE_THREADS

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

      item.Handle();
    }
  }
}

void Init() {
  InitEventType();
  Lock = SDL_CreateMutex();
  WorkSubmitted = SDL_CreateCond();
  SDL_CreateThread(&WorkerThread, "Worker thread", NULL);
}

void Push(void* data, WorkProc worker,
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

#endif

void WorkItem::Handle() {
  Perform(Data);

  WorkItem* copy = (WorkItem*)malloc(sizeof(WorkItem));
  memcpy(copy, this, sizeof(WorkItem));

  SDL_Event evt;
  memset(&evt, 0, sizeof(evt));
  evt.type = WorkCompletedEventType;
  evt.user.data1 = copy;
  SDL_PushEvent(&evt);
}

bool HandleEvent(SDL_Event* evt) {
  if (evt->type != WorkCompletedEventType) return false;

  WorkItem* item = (WorkItem*)evt->user.data1;
  item->OnComplete(item->Data);

  free(item);

  return true;
}
}  // namespace WorkQueue
}  // namespace Impacto