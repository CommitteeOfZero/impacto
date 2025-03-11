#include "sprites.h"
#include "profile_internal.h"
#include "../io/assetpath.h"
#include "../log.h"
#include "../renderer/renderer.h"
#include "../texture/texture.h"
#include <future>

namespace Impacto {
namespace Profile {

static Texture LoadTexture(Io::Stream* stream, std::string name) {
  Texture texture{};
  if (!texture.Load(stream)) {
    ImpLog(LL_Error, LC_Profile,
           "Spritesheet {:s} texture could not be imported, using fallback\n",
           name);
    texture.LoadCheckerboard();
  }

  delete stream;

  return texture;
}

void LoadSpritesheets() {
  EnsurePushMemberOfType("SpriteSheets", LUA_TTABLE);

  std::vector<std::tuple<std::string, std::future<Texture>>> futures;

  PushInitialIndex();
  while (PushNextTableElement() != 0) {
    std::string name(EnsureGetKeyString());

    SpriteSheet& sheet = SpriteSheets[name];
    sheet.DesignWidth = EnsureGetMemberFloat("DesignWidth");
    sheet.DesignHeight = EnsureGetMemberFloat("DesignHeight");

    Io::AssetPath asset = EnsureGetMemberAssetPath("Path");

    Io::Stream* stream;
    IoError err = asset.Open(&stream);
    if (err != IoError_OK) {
      ImpLog(LL_Fatal, LC_Profile, "Could not open spritesheet {:s}\n", name);
      Window->Shutdown();
    }

    futures.emplace_back(std::tuple(
        name, std::async(std::launch::async, LoadTexture, stream, name)));

    Pop();
  }

  for (auto& [name, future] : futures) {
    SpriteSheet& sheet = SpriteSheets[name];
    sheet.Texture = future.get().Submit();
  }

  Pop();

  EnsurePushMemberOfType("Sprites", LUA_TTABLE);

  PushInitialIndex();
  while (PushNextTableElement() != 0) {
    std::string name(EnsureGetKeyString());

    Sprite& sprite = Sprites[name];
    sprite.Sheet = EnsureGetMemberSpriteSheet("Sheet");
    sprite.Bounds = EnsureGetMemberRectF("Bounds");
    if (!TryGetMemberVec2("BaseScale", sprite.BaseScale))
      sprite.BaseScale = glm::vec2(1.0f);

    Pop();
  }

  Pop();
}

}  // namespace Profile
}  // namespace Impacto