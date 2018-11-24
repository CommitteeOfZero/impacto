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
  auto const& _scene3d =
      EnsureGetMemberOfType(Json, "/", "Scene3D", kObjectType);

  MaxRenderables = EnsureGetMemberUint(_scene3d, "/Scene3D", "MaxRenderables");
  AnimationDesignFrameRate =
      EnsureGetMemberFloat(_scene3d, "/Scene3D", "AnimationDesignFrameRate");

  auto const& _defaultCamera =
      EnsureGetMemberOfType(_scene3d, "/Scene3D", "DefaultCamera", kObjectType);

  DefaultCameraPosition =
      EnsureGetMemberVec3(_defaultCamera, "/Scene3D/DefaultCamera", "Position");
  DefaultCameraTarget =
      EnsureGetMemberVec3(_defaultCamera, "/Scene3D/DefaultCamera", "Target");
  DefaultFov =
      EnsureGetMemberFloat(_defaultCamera, "/Scene3D/DefaultCamera", "Fov");

  if (_scene3d.HasMember("AnimationParseBlacklist")) {
    auto const& _animationParseBlacklist = EnsureGetMemberOfType(
        _scene3d, "/Scene3D", "AnimationParseBlacklist", kObjectType);

    for (Value::ConstMemberIterator it = _animationParseBlacklist.MemberBegin();
         it != _animationParseBlacklist.MemberEnd(); ++it) {
      uint32_t model =
          EnsureGetUint(it->name, "/Scene3D/AnimationParseBlacklist/x (name)");
      AssertIs(it->value, "/Scene3D/AnimationParseBlacklist/x", kArrayType);

      for (Value::ConstValueIterator anim = it->value.Begin();
           anim != it->value.End(); ++anim) {
        uint16_t animId =
            EnsureGetUint(*anim, "/Scene3D/AnimationParseBlacklist/x/y");
        AnimationParseBlacklist.emplace_back(model, animId);
      }
    }
  }

  // Characters

  auto const& _characters =
      EnsureGetMemberOfType(_scene3d, "/Scene3D", "Characters", kObjectType);

  for (Value::ConstMemberIterator it = _characters.MemberBegin();
       it != _characters.MemberEnd(); ++it) {
    uint32_t charId = EnsureGetUint(it->name, "/Scene3D/Characters/x (name)");
    AssertIs(it->value, "/Scene3D/Characters/x", kObjectType);

    CharacterDef& character = Characters[charId];
    character.CharacterId = charId;

    auto const& _models = EnsureGetMemberOfType(
        it->value, "/Scene3D/Characters/x", "Models", kArrayType);

    for (Value::ConstValueIterator _model = _models.Begin();
         _model != _models.End(); ++_model) {
      uint32_t modelId =
          EnsureGetUint(*_model, "/Scene3D/Characters/x/Models/y");
      character.Models.push_back(modelId);
      ModelsToCharacters[modelId] = charId;
    }

    auto const& _anims = EnsureGetMemberOfType(
        it->value, "/Scene3D/Characters/x", "Animations", kObjectType);

    for (Value::ConstMemberIterator _anim = _anims.MemberBegin();
         _anim != _anims.MemberEnd(); ++_anim) {
      AssertIs(_anim->value, "/Scene3D/Characters/x/Animations/y", kObjectType);
      uint16_t animId = EnsureGetUint(
          _anim->name, "/Scene3D/Characters/x/Animations/y (name)");
      AnimationDef& animDef = character.Animations[animId];
      animDef.AnimId = animId;
      animDef.CharacterId = charId;

      if (!TryGetMemberFloat(_anim->value, "LoopStart", animDef.LoopStart))
        animDef.LoopStart = 0;
      if (!TryGetMemberFloat(_anim->value, "LoopEnd", animDef.LoopEnd))
        animDef.LoopEnd = 0;
      if (!TryGetMemberBool(_anim->value, "OneShot", animDef.OneShot))
        animDef.OneShot = false;
    }
  }
}

}  // namespace Scene3D
}  // namespace Profile
}  // namespace Impacto