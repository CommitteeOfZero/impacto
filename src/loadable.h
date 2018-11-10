#pragma once

#include "workqueue.h"

namespace Impacto {

enum LoadStatus { LS_Unloaded, LS_Loading, LS_Loaded };

template <typename T>
class Loadable {
  // protected:
  // void LoadSync(uint32_t id);
  // void UnloadSync();
  // void MainThreadOnLoad();

 public:
  LoadStatus Status = LS_Unloaded;

  bool LoadAsync(uint32_t id) {
    if (Status == LS_Loading) {
      // cannot currently cancel a load
      return false;
    }
    Unload();
    NextLoadId = id;
    Status = LS_Loading;
    WorkQueue::Push(this, &LoadWorker, &OnLoaded);
  }

  void Unload() {
    if (Status == LS_Loaded) {
      static_cast<T*>(this)->UnloadSync();
      Status = LS_Unloaded;
    }
  }

 protected:
  uint32_t NextLoadId;

  static void LoadWorker(void* ptr) {
    T* loadable = (T*)ptr;
    loadable->LoadSync(loadable->NextLoadId);
  }

  static void OnLoaded(void* ptr) {
    T* loadable = (T*)ptr;
    loadable->MainThreadOnLoad();
    loadable->Status = LS_Loaded;
  }
};

}  // namespace Impacto