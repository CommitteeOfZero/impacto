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
uint32_t CheckerboardTexture = -1;

void LoadSpritesheets() {
  EnsurePushMemberOfType("SpriteSheets", LUA_TTABLE);

  PushInitialIndex();
  while (PushNextTableElement() != 0) {
    std::string name(EnsureGetKeyString());

    SpriteSheet& sheet = SpriteSheets[name];
    sheet.DesignWidth = EnsureGetMemberFloat("DesignWidth");
    sheet.DesignHeight = EnsureGetMemberFloat("DesignHeight");

    Io::AssetPath asset = EnsureGetMemberAssetPath("Path");

    Io::InputStream* stream = 0;
    Texture texture;
    IoError err = asset.Open(&stream);
    bool isFailed = false;
    if (err != IoError_OK) {
      ImpLog(LL_Fatal, LC_Profile,
             "Could not open spritesheet %s, using fallback\n", name.c_str());
      isFailed = true;
      if (CheckerboardTexture == -1) texture.LoadCheckerboard();
    } else if (!texture.Load(stream)) {
      ImpLog(LL_Error, LC_Profile,
             "Spritesheet %s texture could not be imported, using fallback\n",
             name.c_str());
      isFailed = true;
      delete stream;
      if (CheckerboardTexture == -1) texture.LoadCheckerboard();
    }
    if (isFailed) {
      if (CheckerboardTexture == -1) CheckerboardTexture = texture.Submit();
      sheet.Texture = CheckerboardTexture;
    } else {
      sheet.Texture = texture.Submit();
      delete stream;
    }

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