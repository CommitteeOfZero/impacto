#pragma once

#include "../minilua_impl.h"
#include <initializer_list>
#include <glm/glm.hpp>
#include "../io/assetpath.h"
#include "../util.h"

#include "../spritesheet.h"
#include "../font.h"
#include "../spriteanimation.h"

namespace Impacto {
namespace Profile {

inline lua_State* LuaState;

#define LUA_GET_METHODS(typeName, nativeType)                               \
  bool TryGet##typeName(nativeType& out##typeName);                         \
  nativeType EnsureGet##typeName();                                         \
  bool TryGetMember##typeName(char const* name, nativeType& out##typeName); \
  nativeType EnsureGetMember##typeName(char const* name);                   \
  bool TryGetArrayElement##typeName(nativeType& out##typeName);             \
  nativeType EnsureGetArrayElement##typeName();                             \
  nativeType EnsureGetArrayElementByIndex##typeName(uint32_t index);

void LuaDumpStack();
void Pop();
bool TryPushMember(char const* name);
void EnsurePushMember(char const* name);
void EnsurePushMemberOfType(char const* name, int type);
void AssertIs(int type);
void PushInitialIndex();
int PushNextTableElement();

LUA_GET_METHODS(Bool, bool)
LUA_GET_METHODS(Uint, uint32_t)
LUA_GET_METHODS(Int, int32_t)
LUA_GET_METHODS(Float, float)
LUA_GET_METHODS(String, char const*)

void GetMemberUintArray(uint32_t* arr, uint32_t count, char const* name);
void GetMemberIntArray(int* arr, uint32_t count, char const* name);
void GetMemberFloatArray(float* arr, uint32_t count, char const* name);

LUA_GET_METHODS(Vec2, glm::vec2)
LUA_GET_METHODS(Vec3, glm::vec3)
LUA_GET_METHODS(RectF, RectF)
LUA_GET_METHODS(AssetPath, Io::AssetPath)

void GetMemberVec2Array(glm::vec2* arr, uint32_t count, char const* name);

void GetMemberSpriteArray(Sprite* arr, uint32_t count, char const* name);

LUA_GET_METHODS(Sprite, Sprite)
LUA_GET_METHODS(SpriteSheet, SpriteSheet)
LUA_GET_METHODS(Font, Font*)
LUA_GET_METHODS(Animation, SpriteAnimationDef)

uint32_t EnsureGetKeyUint();
int32_t EnsureGetKeyInt();
char const* EnsureGetKeyString();

void ClearProfileInternal();

#undef LUA_GET_METHODS
#undef LUA_GET_METHODS

}  // namespace Profile
}  // namespace Impacto