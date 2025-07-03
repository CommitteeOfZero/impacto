#pragma once

#include "workqueue.h"

namespace Impacto {

// If you're looking for IRenderable3D loading, it's inside that class itself...
// I'm so sorry

enum class LoadStatus { Unloaded, Loading, Loaded };

template <typename T>
class Loadable {
 public:
  LoadStatus Status = LoadStatus::Unloaded;

  bool LoadAsync(uint32_t id) {
    if (Status == LoadStatus::Loading) {
      // cannot currently cancel a load
      return false;
    }
    Unload();
    NextLoadId = id;
    Status = LoadStatus::Loading;
    WorkQueue::Push(static_cast<T*>(this), &LoadWorker, &OnLoaded);
    return true;
  }

  void Unload() {
    if (Status == LoadStatus::Loaded) {
      static_cast<T*>(this)->UnloadSync();
      Status = LoadStatus::Unloaded;
    }
  }

 protected:
  bool LoadSync(uint32_t id);
  void UnloadSync();
  void MainThreadOnLoad();

  uint32_t NextLoadId;

  static void LoadWorker(void* ptr) {
    T* loadable = (T*)ptr;
    loadable->LoadSync(loadable->NextLoadId);
  }

  static void OnLoaded(void* ptr) {
    T* loadable = (T*)ptr;
    loadable->MainThreadOnLoad();
    loadable->Status = LoadStatus::Loaded;
  }
};

}  // namespace Impacto