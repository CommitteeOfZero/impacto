#include "profile_internal.h"

#include <stack>

#include "../log.h"
#include "../renderer/renderer.h"

#include "sprites.h"
#include "fonts.h"
#include "animations.h"

namespace Impacto {
namespace Profile {

lua_State* LuaState;

void LuaDumpStack() {
  ImpLog(LL_Debug, LC_Profile, "Current stack: \n");
  int top = lua_gettop(LuaState);
  for (int i = 1; i <= top; i++) {
    printf("%d\t%s\t", i, luaL_typename(LuaState, i));
    switch (lua_type(LuaState, i)) {
      case LUA_TNUMBER:
        printf("%g\n", lua_tonumber(LuaState, i));
        break;
      case LUA_TSTRING:
        printf("%s\n", lua_tostring(LuaState, i));
        break;
      case LUA_TBOOLEAN:
        printf("%s\n", (lua_toboolean(LuaState, i) ? "true" : "false"));
        break;
      case LUA_TNIL:
        printf("%s\n", "nil");
        break;
      default:
        printf("%p\n", lua_topointer(LuaState, i));
        break;
    }
  }
}

void Pop() { lua_pop(LuaState, 1); }

bool TryPushMember(char const* name) {
  if (lua_getfield(LuaState, -1, name) == LUA_TNIL) {
    Pop();
    return false;
  }
  return true;
}

void EnsurePushMember(char const* name) {
  bool success = TryPushMember(name);
  if (!success) {
    ImpLog(LL_Fatal, LC_Profile, "Expected %s to have member %s\n",
           lua_tostring(LuaState, -1), name);
    exit(0);
  }
}

void EnsurePushMemberOfType(char const* name, int type) {
  EnsurePushMember(name);
  AssertIs(type);
}

void AssertIs(int type) {
  int actualType = lua_type(LuaState, -1);
  if (actualType != type) {
    exit(0);
  }
}

void PushInitialIndex() { lua_pushnil(LuaState); }

int PushNextTableElement() { return lua_next(LuaState, -2); }

#define LUA_GET_METHODS(typeName, nativeType, typeDesc)                      \
  nativeType EnsureGet##typeName() {                                         \
    nativeType result;                                                       \
    bool success = TryGet##typeName(result);                                 \
    if (!success) {                                                          \
      ImpLog(LL_Fatal, LC_Profile, "Expected %s to be " typeDesc "\n");      \
      Window->Shutdown();                                                    \
    }                                                                        \
    return result;                                                           \
  }                                                                          \
  bool TryGetMember##typeName(char const* name, nativeType& out##typeName) { \
    if (!TryPushMember(name)) return false;                                  \
    bool result = TryGet##typeName(out##typeName);                           \
    Pop();                                                                   \
    return result;                                                           \
  }                                                                          \
  nativeType EnsureGetMember##typeName(char const* name) {                   \
    EnsurePushMember(name);                                                  \
    nativeType result = EnsureGet##typeName();                               \
    Pop();                                                                   \
    return result;                                                           \
  }                                                                          \
  bool TryGetArrayElement##typeName(nativeType& out##typeName) {             \
    bool result = TryGet##typeName(out##typeName);                           \
    return result;                                                           \
  }                                                                          \
  nativeType EnsureGetArrayElement##typeName() {                             \
    nativeType result = EnsureGet##typeName();                               \
    return result;                                                           \
  }                                                                          \
  nativeType EnsureGetArrayElementByIndex##typeName(uint32_t index) {        \
    lua_rawgeti(LuaState, -1, index + 1);                                    \
    nativeType result = EnsureGet##typeName();                               \
    Pop();                                                                   \
    return result;                                                           \
  }

#define TRY_GET_ENTITY(typeName, nativeType, arrayName)        \
  bool TryGet##typeName(nativeType& out##typeName) {           \
    char const* name##typeName;                                \
    if (!TryGetString(name##typeName)) return false;           \
                                                               \
    auto const ref##typeName = arrayName.find(name##typeName); \
    if (ref##typeName == arrayName.end()) return false;        \
                                                               \
    out##typeName = ref##typeName->second;                     \
    return true;                                               \
  }

bool TryGetBool(bool& outBool) {
  if (lua_isboolean(LuaState, -1)) {
    outBool = lua_toboolean(LuaState, -1);
    return true;
  }
  // TODO conversion?

  return false;
}

LUA_GET_METHODS(Bool, bool, "boolean")

bool TryGetUint(uint32_t& outUint) {
  if (lua_isinteger(LuaState, -1)) {
    outUint = lua_tointeger(LuaState, -1);
    return true;
  }
  if (lua_isstring(LuaState, -1)) {
    char* endp;
    outUint = SDL_strtoul(lua_tostring(LuaState, -1), &endp, 10);
    if (endp != 0) return true;
  }

  return false;
}

LUA_GET_METHODS(Uint, uint32_t, "unsigned integer convertible")

bool TryGetInt(int32_t& outInt) {
  if (lua_isinteger(LuaState, -1)) {
    outInt = lua_tointeger(LuaState, -1);
    return true;
  }
  if (lua_isstring(LuaState, -1)) {
    char* endp;
    outInt = SDL_strtol(lua_tostring(LuaState, -1), &endp, 10);
    if (endp != 0) return true;
  }

  return false;
}

void GetMemberIntArray(int* arr, uint32_t count, char const* name) {
  EnsurePushMemberOfType(name, LUA_TTABLE);

  if (lua_rawlen(LuaState, -1) != count) {
    ImpLog(LL_Fatal, LC_Profile, "Expected to have %d ints for %s\n", count,
           name);
    Window->Shutdown();
  }

  PushInitialIndex();
  while (PushNextTableElement()) {
    int i = EnsureGetKeyInt() - 1;
    arr[i] = EnsureGetArrayElementInt();
    Pop();
  }

  Pop();
}

LUA_GET_METHODS(Int, int32_t, "signed integer convertible")

bool TryGetFloat(float& outFloat) {
  if (lua_isnumber(LuaState, -1)) {
    outFloat = lua_tonumber(LuaState, -1);
  }
  if (lua_isstring(LuaState, -1)) {
    char* endp;
    outFloat = SDL_strtod(lua_tostring(LuaState, -1), &endp);
    if (endp != 0) return true;
  }

  return false;
}

void GetMemberFloatArray(float* arr, uint32_t count, char const* name) {
  EnsurePushMemberOfType(name, LUA_TTABLE);

  if (lua_rawlen(LuaState, -1) != count) {
    ImpLog(LL_Fatal, LC_Profile, "Expected to have %d floats for %s\n", count,
           name);
    Window->Shutdown();
  }

  PushInitialIndex();
  while (PushNextTableElement()) {
    int i = EnsureGetKeyInt() - 1;
    arr[i] = EnsureGetArrayElementFloat();
    Pop();
  }

  Pop();
}

LUA_GET_METHODS(Float, float, "float convertible")

bool TryGetString(char const*& outString) {
  if (!lua_isstring(LuaState, -1)) return false;
  outString = lua_tostring(LuaState, -1);
  return true;
}

char const* EnsureGetKeyString() { return lua_tostring(LuaState, -2); }

int32_t EnsureGetKeyInt() { return lua_tointeger(LuaState, -2); }

uint32_t EnsureGetKeyUint() { return (uint32_t)lua_tointeger(LuaState, -2); }

LUA_GET_METHODS(String, char const*, "string")

bool TryGetVec2(glm::vec2& outVec2) {
  if (!lua_istable(LuaState, -1)) return false;

  return TryGetMemberFloat("X", outVec2.x) && TryGetMemberFloat("Y", outVec2.y);
}

void GetMemberVec2Array(glm::vec2* arr, uint32_t count, char const* name) {
  EnsurePushMemberOfType(name, LUA_TTABLE);

  if (lua_rawlen(LuaState, -1) != count) {
    ImpLog(LL_Fatal, LC_Profile, "Expected to have %d vec2 for %s\n", count,
           name);
    Window->Shutdown();
  }

  PushInitialIndex();
  while (PushNextTableElement()) {
    int i = EnsureGetKeyInt() - 1;
    arr[i] = EnsureGetArrayElementVec2();
    Pop();
  }

  Pop();
}

LUA_GET_METHODS(Vec2, glm::vec2, "Vec2")

bool TryGetVec3(glm::vec3& outVec3) {
  if (!lua_istable(LuaState, -1)) return false;

  return TryGetMemberFloat("X", outVec3.x) &&
         TryGetMemberFloat("Y", outVec3.y) && TryGetMemberFloat("Z", outVec3.z);
}

LUA_GET_METHODS(Vec3, glm::vec3, "Vec3")

bool TryGetRectF(RectF& outRectF) {
  if (!lua_istable(LuaState, -1)) return false;

  return TryGetMemberFloat("X", outRectF.X) &&
         TryGetMemberFloat("Y", outRectF.Y) &&
         TryGetMemberFloat("Width", outRectF.Width) &&
         TryGetMemberFloat("Height", outRectF.Height);
}

LUA_GET_METHODS(RectF, RectF, "RectF")

bool TryGetAssetPath(Io::AssetPath& outPath) {
  if (lua_isstring(LuaState, -1)) {
    outPath.Mount = "";
    outPath.Id = 0;
    outPath.FileName = lua_tostring(LuaState, -1);
    return true;
  } else {
    if (!lua_istable(LuaState, -1)) return false;

    if (!TryGetMemberUint("Id", outPath.Id)) return false;
    char const* _mount;
    if (!TryGetMemberString("Mount", _mount)) return false;
    outPath.Mount = _mount;
    return true;
  }
}

LUA_GET_METHODS(AssetPath, Io::AssetPath, "AssetPath")

void GetMemberSpriteArray(Sprite* arr, uint32_t count, char const* name) {
  EnsurePushMemberOfType(name, LUA_TTABLE);

  int actualCount = lua_rawlen(LuaState, -1);
  if (actualCount != count) {
    ImpLog(LL_Fatal, LC_Profile, "Expected to have %d sprites for %s, got %d\n",
           count, name, actualCount);
    Window->Shutdown();
  }

  PushInitialIndex();
  while (PushNextTableElement()) {
    int i = EnsureGetKeyInt() - 1;
    arr[i] = EnsureGetArrayElementSprite();
    Pop();
  }

  Pop();
}

TRY_GET_ENTITY(Sprite, Sprite, Sprites)
LUA_GET_METHODS(Sprite, Sprite, "Sprite")

TRY_GET_ENTITY(SpriteSheet, SpriteSheet, SpriteSheets)
LUA_GET_METHODS(SpriteSheet, SpriteSheet, "SpriteSheet")

TRY_GET_ENTITY(Font, Font*, Fonts)
LUA_GET_METHODS(Font, Font*, "Font")

TRY_GET_ENTITY(Animation, SpriteAnimationDef, Animations)
LUA_GET_METHODS(Animation, SpriteAnimationDef, "Animation")

void ClearProfileInternal() { lua_close(LuaState); }

}  // namespace Profile
}  // namespace Impacto