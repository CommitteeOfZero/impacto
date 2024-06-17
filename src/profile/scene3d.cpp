#include "scene3d.h"
#include "profile_internal.h"

namespace Impacto {
namespace Profile {
namespace Scene3D {

void Configure() {
  EnsurePushMemberOfType("Scene3D", LUA_TTABLE);

  Version = LKMVersion::_from_integral_unchecked(EnsureGetMemberInt("Version"));

  MaxRenderables = EnsureGetMemberUint("MaxRenderables");
  AnimationDesignFrameRate = EnsureGetMemberFloat("AnimationDesignFrameRate");

  {
    EnsurePushMemberOfType("DefaultCamera", LUA_TTABLE);

    DefaultCameraPosition = EnsureGetMemberVec3("Position");
    DefaultCameraTarget = EnsureGetMemberVec3("Target");
    DefaultFov = EnsureGetMemberFloat("Fov");

    Pop();
  }

  if (TryPushMember("AnimationParseBlacklist")) {
    AssertIs(LUA_TTABLE);

    PushInitialIndex();
    while (PushNextTableElement() != 0) {
      const uint32_t model = EnsureGetKeyUint();
      AssertIs(LUA_TTABLE);
      PushInitialIndex();
      while (PushNextTableElement() != 0) {
        AnimationParseBlacklist.emplace_back(model, EnsureGetArrayElementInt());
        Pop();
      }
      Pop();
    }

    Pop();
  }

  // Characters

  {
    EnsurePushMemberOfType("Characters", LUA_TTABLE);

    PushInitialIndex();
    while (PushNextTableElement() != 0) {
      const uint32_t charId = EnsureGetKeyUint();

      CharacterDef& character = Characters[charId];
      character.CharacterId = charId;
      character.IdleAnimation = EnsureGetMemberInt("IdleAnimation");

      {
        EnsurePushMemberOfType("Models", LUA_TTABLE);

        PushInitialIndex();
        while (PushNextTableElement() != 0) {
          uint32_t modelId = EnsureGetArrayElementUint();
          character.Models.push_back(modelId);
          ModelsToCharacters[modelId] = charId;
          Pop();
        }

        Pop();
      }

      {
        EnsurePushMemberOfType("Animations", LUA_TTABLE);

        PushInitialIndex();
        while (PushNextTableElement() != 0) {
          uint16_t animId = EnsureGetKeyInt();

          AnimationDef& animDef = character.Animations[animId];
          animDef.AnimId = animId;
          animDef.CharacterId = charId;

          if (!TryGetMemberFloat("LoopStart", animDef.LoopStart))
            animDef.LoopStart = 0;
          if (!TryGetMemberFloat("LoopEnd", animDef.LoopEnd))
            animDef.LoopEnd = 0;
          if (!TryGetMemberBool("OneShot", animDef.OneShot))
            animDef.OneShot = false;

          Pop();
        }

        Pop();
      }

      Pop();
    }

    Pop();
  }

  Pop();
}

}  // namespace Scene3D
}  // namespace Profile
}  // namespace Impacto