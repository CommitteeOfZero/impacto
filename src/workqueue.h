#pragma once

#include <SDL.h>

namespace Impacto {

typedef void (*WorkProc)(void* data);
typedef void (*WorkCompletionCallbackProc)(void* data);

void WorkQueueInit();
// Push work onto the background thread. worker(data) will be called on the
// background thread in FIFO fashion. Completion will be posted onto SDL's event
// queue.
void WorkQueuePush(void* data, WorkProc worker,
                   WorkCompletionCallbackProc completionCallback);
// Call this in the event loop to perform work completion callbacks on the main
// thread at the start of every frame. Returns true if event was handled, false
// if not.
bool WorkQueueHandleEvent(SDL_Event* evt);

}  // namespace Impacto