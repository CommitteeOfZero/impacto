#pragma once

#include <SDL.h>

namespace Impacto {
namespace WorkQueue {
using WorkProc = auto (*)(void* data) -> void;
using WorkCompletionCallbackProc = auto (*)(void* data) -> void;

void Init();
// Push work onto the background thread. worker(data) will be called on the
// background thread in FIFO fashion. Completion will be posted onto SDL's event
// queue.
void Push(void* data, WorkProc worker,
          WorkCompletionCallbackProc completionCallback);
// Call this in the event loop to perform work completion callbacks on the main
// thread at the start of every frame. Returns true if event was handled, false
// if not.
bool HandleEvent(SDL_Event* evt);
}  // namespace WorkQueue
}  // namespace Impacto