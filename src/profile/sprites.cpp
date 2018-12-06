#include "sprites.h"
#include "profile_internal.h"
#include "../io/assetpath.h"
#include "../log.h"
#include "../window.h"
#include "../texture/texture.h"

namespace Impacto {
namespace Profile {

ska::flat_hash_map<std::string, SpriteSheet> SpriteSheets;
ska::flat_hash_map<std::string, Sprite> Sprites;

void LoadSpritesheets() {
  EnsurePushMemberOfType("SpriteSheets", kObjectType);

  auto const& _spritesheets = TopVal();
  for (Value::ConstMemberIterator it = _spritesheets.MemberBegin();
       it != _spritesheets.MemberEnd(); it++) {
    std::string name(EnsureGetKeyString(it));

    EnsurePushMemberIteratorOfType(it, kObjectType);

    SpriteSheet& sheet = SpriteSheets[name];
    sheet.DesignWidth = EnsureGetMemberFloat("DesignWidth");
    sheet.DesignHeight = EnsureGetMemberFloat("DesignHeight");

    Io::AssetPath asset = EnsureGetMemberAssetPath("Path");

    Io::InputStream* stream;
    IoError err = asset.Open(&stream);
    if (err != IoError_OK) {
      ImpLog(LL_Fatal, LC_Profile, "Could not open spritesheet %s\n",
             name.c_str());
      Window::Shutdown();
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

  EnsurePushMemberOfType("Sprites", kObjectType);

  auto const& _sprites = TopVal();
  for (Value::ConstMemberIterator it = _sprites.MemberBegin();
       it != _sprites.MemberEnd(); it++) {
    std::string name(EnsureGetKeyString(it));

    EnsurePushMemberIteratorOfType(it, kObjectType);

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