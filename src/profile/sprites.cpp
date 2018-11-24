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
  auto const& _spritesheets =
      EnsureGetMemberOfType(Json, "/", "SpriteSheets", kObjectType);
  auto const& _sprites =
      EnsureGetMemberOfType(Json, "/", "Sprites", kObjectType);

  for (Value::ConstMemberIterator it = _spritesheets.MemberBegin();
       it != _spritesheets.MemberEnd(); ++it) {
    std::string name(EnsureGetString(it->name, "/SpriteSheets/x (name)"));
    AssertIs(it->value, "/SpriteSheets/x", kObjectType);

    ImpLog(LL_Debug, LC_Profile, "Loading spritesheet %s\n", name.c_str());

    SpriteSheet& sheet = SpriteSheets[name];

    sheet.DesignWidth =
        EnsureGetMemberFloat(it->value, "/SpriteSheets/x", "DesignWidth");
    sheet.DesignHeight =
        EnsureGetMemberFloat(it->value, "/SpriteSheets/x", "DesignHeight");

    Io::AssetPath asset =
        EnsureGetMemberAssetPath(it->value, "/SpriteSheets/x", "Path");

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
  }

  for (Value::ConstMemberIterator it = _sprites.MemberBegin();
       it != _sprites.MemberEnd(); ++it) {
    std::string name(EnsureGetString(it->name, "/Sprites/x (name)"));
    AssertIs(it->value, "/Sprites/x", kObjectType);

    ImpLog(LL_Debug, LC_Profile, "Loading sprite %s\n", name.c_str());

    Sprite& sprite = Sprites[name];

    char const* sheetName =
        EnsureGetMemberString(it->value, "/Sprites/x", "Sheet");

    auto const sheetRef = SpriteSheets.find(sheetName);
    if (sheetRef == SpriteSheets.end()) {
      ImpLog(LL_Fatal, LC_Profile, "No spritesheet %s (used by %s)\n",
             sheetName, name.c_str());
      Window::Shutdown();
    }

    sprite.Sheet = sheetRef->second;
    sprite.Bounds = EnsureGetMemberRectF(it->value, "/Sprites/x", "Bounds");
    if (!TryGetMemberVec2(it->value, "BaseScale", sprite.BaseScale)) {
      sprite.BaseScale = glm::vec2(1.0f);
    }
  }
}

}  // namespace Profile
}  // namespace Impacto