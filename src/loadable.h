#pragma once

#include "workqueue.h"

namespace Impacto {

// If you're looking for IRenderable3D loading, it's inside that class itself...
// I'm so sorry

enum class LoadStatus { Unloaded, Loading, Loaded };

template <typename T, typename LoadResult, typename... LoadArgs>
class Loadable {
 public:
  LoadStatus Status = LoadStatus::Unloaded;

  bool LoadAsync(LoadArgs... args) {
    if (Status == LoadStatus::Loading) {
      // cannot currently cancel a load
      return false;
    }
    Unload();
    NextLoadArgs = std::make_tuple(args...);
    Status = LoadStatus::Loading;
    WorkQueue::Push(this, &LoadWorker, &OnLoaded);
    return true;
  }

  void Unload() {
    if (Status == LoadStatus::Loaded) {
      static_cast<T*>(this)->UnloadSync();
      Status = LoadStatus::Unloaded;
    }
  }

 protected:
  LoadResult LoadSync(LoadArgs... args);
  void UnloadSync();
  void MainThreadOnLoad(LoadResult result);

 private:
  std::tuple<LoadArgs...> NextLoadArgs;
  LoadResult LastLoadResult;

  static void LoadWorker(void* ptr) {
    T* loadable = (T*)ptr;
    loadable->LastLoadResult = std::apply(
        &T::LoadSync,
        std::tuple_cat(std::make_tuple(loadable), loadable->NextLoadArgs));
  }

  static void OnLoaded(void* ptr) {
    T* loadable = (T*)ptr;
    loadable->MainThreadOnLoad(loadable->LastLoadResult);
    loadable->Status = LoadStatus::Loaded;
  }
};

}  // namespace Impacto