#pragma once

#include <rapidjson/document.h>
#include <initializer_list>
#include <glm/glm.hpp>
#include "../io/assetpath.h"
#include "../util.h"

#include "../spritesheet.h"
#include "../font.h"
#include "../spriteanimation.h"

using namespace rapidjson;

namespace Impacto {
namespace Profile {

extern Document Json;
extern Value GlobalNull;

#define JSON_GET_METHODS(typeName, nativeType)                              \
  bool TryGet##typeName(nativeType& out##typeName);                         \
  nativeType EnsureGet##typeName();                                         \
  bool TryGetMember##typeName(char const* name, nativeType& out##typeName); \
  nativeType EnsureGetMember##typeName(char const* name);                   \
  bool TryGetArrayElement##typeName(uint32_t index,                         \
                                    nativeType& out##typeName);             \
  nativeType EnsureGetArrayElement##typeName(uint32_t index);

Value const& TopVal();
void Pop();
void PushArrayElement(uint32_t i);
bool TryPushMember(char const* name);
void EnsurePushMember(char const* name);
void EnsurePushMemberOfType(char const* name, Type type);
void AssertIs(Type type);
void AssertIsOneOf(std::initializer_list<Type> types);
void PushMemberIterator(Value::ConstMemberIterator it);
void EnsurePushMemberIteratorOfType(Value::ConstMemberIterator it, Type type);

JSON_GET_METHODS(Bool, bool)
JSON_GET_METHODS(Uint, uint32_t)
JSON_GET_METHODS(Int, int32_t)
JSON_GET_METHODS(Float, float)
JSON_GET_METHODS(String, char const*)

JSON_GET_METHODS(Vec2, glm::vec2)
JSON_GET_METHODS(Vec3, glm::vec3)
JSON_GET_METHODS(RectF, RectF)
JSON_GET_METHODS(AssetPath, Io::AssetPath)

JSON_GET_METHODS(Sprite, Sprite)
JSON_GET_METHODS(SpriteSheet, SpriteSheet)
JSON_GET_METHODS(Font, Font*)
JSON_GET_METHODS(Animation, SpriteAnimationDef)

uint32_t EnsureGetKeyUint(Value::ConstMemberIterator it);
int32_t EnsureGetKeyInt(Value::ConstMemberIterator it);
char const* EnsureGetKeyString(Value::ConstMemberIterator it);

void LoadJsonString(char const* str);
void ClearJsonProfileInternal();

}  // namespace Profile
}  // namespace Impacto