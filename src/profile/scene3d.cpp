#include "scene3d.h"
#include "profile_internal.h"

namespace Impacto {
namespace Profile {
namespace Scene3D {

void Configure() {
  EnsurePushMemberOfType("Scene3D", LUA_TTABLE);

  Version =
      LKMVersion::_from_integral_unchecked(EnsureGetMember<int>("Version"));

  MaxRenderables = EnsureGetMember<uint32_t>("MaxRenderables");
  AnimationDesignFrameRate = EnsureGetMember<float>("AnimationDesignFrameRate");

  {
    EnsurePushMemberOfType("DefaultCamera", LUA_TTABLE);

    DefaultCameraPosition = EnsureGetMember<glm::vec3>("Position");
    DefaultCameraTarget = EnsureGetMember<glm::vec3>("Target");
    DefaultFov = EnsureGetMember<float>("Fov");

    Pop();
  }

  if (TryPushMember("AnimationParseBlacklist")) {
    AssertIs(LUA_TTABLE);

    PushInitialIndex();
    while (PushNextTableElement() != 0) {
      const uint32_t model = EnsureGetKey<uint32_t>();
      AssertIs(LUA_TTABLE);
      PushInitialIndex();
      while (PushNextTableElement() != 0) {
        AnimationParseBlacklist.emplace_back(model,
                                             EnsureGetArrayElement<int>());
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
      const uint32_t charId = EnsureGetKey<uint32_t>();

      CharacterDef& character = Characters[charId];
      character.CharacterId = charId;
      character.IdleAnimation = EnsureGetMember<uint16_t>("IdleAnimation");

      {
        EnsurePushMemberOfType("Models", LUA_TTABLE);

        PushInitialIndex();
        while (PushNextTableElement() != 0) {
          uint32_t modelId = EnsureGetArrayElement<uint32_t>();
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
          uint16_t animId = EnsureGetKey<uint16_t>();

          AnimationDef& animDef = character.Animations[animId];
          animDef.AnimId = animId;
          animDef.CharacterId = charId;

          if (!TryGetMember<float>("LoopStart", animDef.LoopStart))
            animDef.LoopStart = 0;
          if (!TryGetMember<float>("LoopEnd", animDef.LoopEnd))
            animDef.LoopEnd = 0;
          if (!TryGetMember<bool>("OneShot", animDef.OneShot))
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