#include "scene3d.h"
#include "profile_internal.h"

namespace Impacto {
namespace Profile {
namespace Scene3D {

int MaxRenderables;

glm::vec3 DefaultCameraPosition;
glm::vec3 DefaultCameraTarget;
float DefaultFov;

float AnimationDesignFrameRate;

std::vector<std::pair<uint32_t, uint16_t>> AnimationParseBlacklist;

ska::flat_hash_map<uint32_t, CharacterDef> Characters;
ska::flat_hash_map<uint32_t, uint32_t> ModelsToCharacters;

void Configure() {
  EnsurePushMemberOfType("Scene3D", kObjectType);

  MaxRenderables = EnsureGetMemberUint("MaxRenderables");
  AnimationDesignFrameRate = EnsureGetMemberFloat("AnimationDesignFrameRate");

  {
    EnsurePushMemberOfType("DefaultCamera", kObjectType);

    DefaultCameraPosition = EnsureGetMemberVec3("Position");
    DefaultCameraTarget = EnsureGetMemberVec3("Target");
    DefaultFov = EnsureGetMemberFloat("Fov");

    Pop();
  }

  if (TryPushMember("AnimationParseBlacklist")) {
    AssertIs(kObjectType);

    auto const& _animationParseBlacklist = TopVal();
    for (Value::ConstMemberIterator it = _animationParseBlacklist.MemberBegin();
         it != _animationParseBlacklist.MemberEnd(); it++) {
      uint32_t model = EnsureGetKeyUint(it);
      EnsurePushMemberIteratorOfType(it, kArrayType);

      uint32_t animCount = TopVal().Size();
      for (uint32_t i = 0; i < animCount; i++) {
        AnimationParseBlacklist.emplace_back(model,
                                             EnsureGetArrayElementUint(i));
      }

      Pop();
    }

    Pop();
  }

  // Characters

  {
    EnsurePushMemberOfType("Characters", kObjectType);

    auto const& _characters = TopVal();
    for (Value::ConstMemberIterator it = _characters.MemberBegin();
         it != _characters.MemberEnd(); it++) {
      uint32_t charId = EnsureGetKeyUint(it);

      EnsurePushMemberIteratorOfType(it, kObjectType);

      CharacterDef& character = Characters[charId];
      character.CharacterId = charId;

      {
        EnsurePushMemberOfType("Models", kArrayType);

        uint32_t modelCount = TopVal().Size();
        for (uint32_t i = 0; i < modelCount; i++) {
          uint32_t modelId = EnsureGetArrayElementUint(i);
          character.Models.push_back(modelId);
          ModelsToCharacters[modelId] = charId;
        }

        Pop();
      }

      {
        EnsurePushMemberOfType("Animations", kObjectType);

        auto const& _anims = TopVal();
        for (Value::ConstMemberIterator anim = _anims.MemberBegin();
             anim != _anims.MemberEnd(); anim++) {
          uint16_t animId = EnsureGetKeyUint(anim);

          EnsurePushMemberIteratorOfType(anim, kObjectType);

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