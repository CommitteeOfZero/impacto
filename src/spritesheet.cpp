#include "spritesheet.h"
#include "renderer/renderer.h"
#include <mutex>
#include <shared_mutex>

namespace Impacto {

static std::shared_mutex MapMutex;

bool SpriteLoader::LoadSync(int surfId, int archiveId, int fileId) {
  auto path = Io::AssetPathKey{.MountId = static_cast<uint32_t>(archiveId),
                               .Id = static_cast<uint32_t>(fileId)};

  auto innerTextureIdIter = Renderer->SheetPathToId.find(path);
  if (innerTextureIdIter == Renderer->SheetPathToId.end()) {
    return false;
  }
  InnerTextureId = innerTextureIdIter->second;

  {
    std::unique_lock writeLock(MapMutex);
    Renderer->SurfToId.try_emplace(surfId, path);
  }

  Io::AssetPath pathRes = Io::AssetPathKey::KeyToAssetPath(path);
  Io::Stream* stream;
  IoError err = pathRes.Open(&stream);
  if (err != IoError_OK) {
    Window->Shutdown();
  }

  LoadedTexture.Load(stream);
  delete stream;
  return true;
}

void SpriteLoader::MainThreadOnLoad(bool result) {
  if (!result) return;
  Renderer->LookupTextureIdToTexture.try_emplace(InnerTextureId,
                                                 LoadedTexture.Submit());
}
}  // namespace Impacto