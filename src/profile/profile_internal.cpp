#include "profile_internal.h"
#include "../log.h"
#include "../renderer/renderer.h"

#include "sprites.h"
#include "fonts.h"
#include "animations.h"

namespace Impacto {
namespace Profile {

void LuaDumpStack() {
  std::string callStack;
  int top = lua_gettop(LuaState);
  for (int i = 1; i <= top; i++) {
    fmt::format_to(std::back_inserter(callStack), "{:d}\t{}\t", i,
                   luaL_typename(LuaState, i));
    switch (lua_type(LuaState, i)) {
      case LUA_TNUMBER:
        fmt::format_to(std::back_inserter(callStack), "{:g}",
                       lua_tonumber(LuaState, i));
        break;
      case LUA_TSTRING:
        fmt::format_to(std::back_inserter(callStack), "{:s}",
                       lua_tostring(LuaState, i));
        break;
      case LUA_TBOOLEAN:
        fmt::format_to(std::back_inserter(callStack), "{}",
                       lua_toboolean(LuaState, i) ? "true" : "false");
        break;
      case LUA_TNIL:
        fmt::format_to(std::back_inserter(callStack), "nil");
        break;
      default:
        fmt::format_to(std::back_inserter(callStack), "{}",
                       lua_topointer(LuaState, i));
        break;
    }
  }
  ImpLog(LogLevel::Debug, LogChannel::Profile, "Current stack: \n{}",
         callStack);
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
    ImpLog(LogLevel::Fatal, LogChannel::Profile, "Expected member {:s}\n",
           name);
    exit(1);
  }
}

void EnsurePushMemberOfType(char const* name, int type) {
  EnsurePushMember(name);
  AssertIs(type);
}

void AssertIs(int type) {
  int actualType = lua_type(LuaState, -1);
  if (actualType != type) {
    exit(1);
  }
}

void PushInitialIndex() { lua_pushnil(LuaState); }

int PushNextTableElement() { return lua_next(LuaState, -2); }

template <>
bool TryGet<Io::AssetPath>(Io::AssetPath& outPath) {
  if (lua_isstring(LuaState, -1)) {
    outPath.Mount = "";
    outPath.Id = 0;
    outPath.FileName = lua_tostring(LuaState, -1);
    return true;
  } else {
    if (!lua_istable(LuaState, -1)) return false;

    if (!TryGetMember("Id", outPath.Id)) return false;
    char const* _mount;
    if (!TryGetMember("Mount", _mount)) return false;
    outPath.Mount = _mount;
    return true;
  }
}

template <>
bool TryGet<RectF>(RectF& outRectF) {
  if (!lua_istable(LuaState, -1)) return false;

  return TryGetMember("X", outRectF.X) && TryGetMember("Y", outRectF.Y) &&
         TryGetMember("Width", outRectF.Width) &&
         TryGetMember("Height", outRectF.Height);
}

template <>
bool TryGet<glm::vec3>(glm::vec3& outVec3) {
  if (!lua_istable(LuaState, -1)) return false;

  return TryGetMember("X", outVec3.x) && TryGetMember("Y", outVec3.y) &&
         TryGetMember("Z", outVec3.z);
}

template <>
bool TryGet<glm::vec2>(glm::vec2& outVec2) {
  if (!lua_istable(LuaState, -1)) return false;

  return TryGetMember("X", outVec2.x) && TryGetMember("Y", outVec2.y);
}

template <>
bool TryGet<glm::u16vec2>(glm::u16vec2& outVec2) {
  if (!lua_istable(LuaState, -1)) return false;

  return TryGetMember("X", outVec2.x) && TryGetMember("Y", outVec2.y);
}

template <>
bool TryGet<DialogueColorPair>(DialogueColorPair& outColor) {
  if (!lua_istable(LuaState, -1)) return false;

  return TryGetMember("TextColor", outColor.TextColor) &&
         TryGetMember("OutlineColor", outColor.OutlineColor);
}

template <>
bool TryGet<bool>(bool& outBool) {
  if (lua_isboolean(LuaState, -1)) {
    outBool = lua_toboolean(LuaState, -1);
    return true;
  }
  // TODO conversion?

  return false;
}

void ClearProfileInternal() { lua_close(LuaState); }

}  // namespace Profile
}  // namespace Impacto