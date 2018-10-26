#pragma once

#include "workqueue.h"

namespace Impacto {

enum LoadStatus { LS_Unloaded, LS_Loading, LS_Loaded };

template <typename T>
class Loadable {
 public:
  ~Loadable() {}

  // void Load(uint32_t id);
  // void Unload();
  // void MainThreadOnLoad();

  LoadStatus Status = LS_Unloaded;

  bool LoadAsync(uint32_t id) {
    if (Status == LS_Loading) {
      // cannot currently cancel a load
      return false;
    }
    if (Status == LS_Loaded) {
      static_cast<T*>(this)->Unload();
      Status = LS_Unloaded;
    }
    NextLoadId = id;
    Status = LS_Loading;
    WorkQueuePush(this, &LoadWorker, &OnLoaded);
  }

 protected:
  uint32_t NextLoadId;

  static void LoadWorker(void* ptr) {
    T* loadable = (T*)ptr;
    loadable->Load(loadable->NextLoadId);
  }

  static void OnLoaded(void* ptr) {
    T* loadable = (T*)ptr;
    loadable->MainThreadOnLoad();
    loadable->Status = LS_Loaded;
  }
};

}  // namespace Impacto