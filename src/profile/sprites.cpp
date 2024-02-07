#include "sprites.h"
#include "profile_internal.h"
#include "../io/assetpath.h"
#include "../log.h"
#include "../renderer/renderer.h"
#include "../texture/texture.h"

namespace Impacto {
namespace Profile {

ska::flat_hash_map<std::string, SpriteSheet> SpriteSheets;
ska::flat_hash_map<std::string, Sprite> Sprites;

void LoadSpritesheets() {
  EnsurePushMemberOfType("SpriteSheets", LUA_TTABLE);

  PushInitialIndex();
  while (PushNextTableElement() != 0) {
    std::string name(EnsureGetKeyString());

    SpriteSheet& sheet = SpriteSheets[name];
    sheet.DesignWidth = EnsureGetMemberFloat("DesignWidth");
    sheet.DesignHeight = EnsureGetMemberFloat("DesignHeight");

    Io::AssetPath asset = EnsureGetMemberAssetPath("Path");

    Io::InputStream* stream;
    IoError err = asset.Open(&stream);
    if (err != IoError_OK) {
      ImpLog(LL_Fatal, LC_Profile, "Could not open spritesheet %s\n",
             name.c_str());
      Window->Shutdown();
    }
    Texture texture;
    if (!texture.Load(stream)) {
      ImpLog(LL_Error, LC_Profile,
             "Spritesheet %s texture could not be imported, using fallback\n",
             name.c_str());
      texture.LoadCheckerboard();
    }
    delete stream;
    sheet.Texture = texture.Submit();

    Pop();
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