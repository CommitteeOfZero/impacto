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
      AssertIs(EnsureGetMember(Json, "/", "Scene3D"), "/Scene3D", kObjectType);

  MaxRenderables =
      EnsureGetInt(EnsureGetMember(_scene3d, "/Scene3D", "MaxRenderables"),
                   "/Scene3D/MaxRenderables");
  AnimationDesignFrameRate = EnsureGetFloat(
      EnsureGetMember(_scene3d, "/Scene3D", "AnimationDesignFrameRate"),
      "/Scene3D/AnimationDesignFrameRate");

  auto const& _defaultCamera =
      AssertIs(EnsureGetMember(_scene3d, "/Scene3D", "DefaultCamera"),
               "/Scene3D/DefaultCamera", kObjectType);

  DefaultCameraPosition = EnsureGetVec3(
      EnsureGetMember(_defaultCamera, "/Scene3D/DefaultCamera", "Position"),
      "/Scene3D/DefaultCamera/Position");
  DefaultCameraTarget = EnsureGetVec3(
      EnsureGetMember(_defaultCamera, "/Scene3D/DefaultCamera", "Target"),
      "/Scene3D/DefaultCamera/Target");
  DefaultFov = EnsureGetFloat(
      EnsureGetMember(_defaultCamera, "/Scene3D/DefaultCamera", "Fov"),
      "/Scene3D/DefaultCamera/Fov");

  if (_scene3d.HasMember("AnimationParseBlacklist")) {
    auto const& _animationParseBlacklist =
        AssertIs(_scene3d["AnimationParseBlacklist"],
                 "/Scene3D/AnimationParseBlacklist", kObjectType);

    for (Value::ConstMemberIterator it = _animationParseBlacklist.MemberBegin();
         it != _animationParseBlacklist.MemberEnd(); ++it) {
      uint32_t model =
          EnsureGetInt(it->name, "/Scene3D/AnimationParseBlacklist/x (name)");
      AssertIs(it->value, "/Scene3D/AnimationParseBlacklist/x", kArrayType);

      for (Value::ConstValueIterator anim = it->value.Begin();
           anim != it->value.End(); ++anim) {
        uint16_t animId =
            EnsureGetInt(*anim, "/Scene3D/AnimationParseBlacklist/x/y");
        AnimationParseBlacklist.emplace_back(model, animId);
      }
    }
  }

  // Characters

  auto const& _characters =
      AssertIs(EnsureGetMember(_scene3d, "/Scene3D", "Characters"),
               "/Scene3D/Characters", kObjectType);

  for (Value::ConstMemberIterator it = _characters.MemberBegin();
       it != _characters.MemberEnd(); ++it) {
    uint32_t charId = EnsureGetInt(it->name, "/Scene3D/Characters/x (name)");
    AssertIs(it->value, "/Scene3D/Characters/x", kObjectType);

    CharacterDef& character = Characters[charId];
    character.CharacterId = charId;

    auto const& _models =
        AssertIs(EnsureGetMember(it->value, "/Scene3D/Characters/x", "Models"),
                 "/Scene3D/Characters/x/Models", kArrayType);

    for (Value::ConstValueIterator _model = _models.Begin();
         _model != _models.End(); ++_model) {
      uint32_t modelId =
          EnsureGetInt(*_model, "/Scene3D/Characters/x/Models/y");
      character.Models.push_back(modelId);
      ModelsToCharacters[modelId] = charId;
    }

    auto const& _anims = AssertIs(
        EnsureGetMember(it->value, "/Scene3D/Characters/x", "Animations"),
        "/Scene3D/Characters/x/Animations", kObjectType);

    for (Value::ConstMemberIterator _anim = _anims.MemberBegin();
         _anim != _anims.MemberEnd(); ++_anim) {
      AssertIs(_anim->value, "/Scene3D/Characters/x/Animations/y", kObjectType);
      uint16_t animId = EnsureGetInt(
          _anim->name, "/Scene3D/Characters/x/Animations/y (name)");
      AnimationDef& animDef = character.Animations[animId];
      animDef.AnimId = animId;
      animDef.CharacterId = charId;
      {
        auto const mit = _anim->value.FindMember("OneShot");
        if (mit != _anim->value.MemberEnd() && mit->value.IsBool())
          animDef.OneShot = mit->value.GetBool();
      }
      {
        auto const mit = _anim->value.FindMember("LoopStart");
        if (mit != _anim->value.MemberEnd() && mit->value.IsNumber())
          animDef.LoopStart = mit->value.GetFloat();
      }
      {
        auto const mit = _anim->value.FindMember("LoopEnd");
        if (mit != _anim->value.MemberEnd() && mit->value.IsNumber())
          animDef.LoopEnd = mit->value.GetFloat();
      }
    }
  }
}

}  // namespace Scene3D
}  // namespace Profile
}  // namespace Impacto