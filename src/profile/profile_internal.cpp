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
    std::string error = fmt::format("Expected member {:s}", name);
    ImpLog(LogLevel::Fatal, LogChannel::Profile, "{:s}\n", error);
    throw std::runtime_error(error);
  }
}

void EnsurePushMemberOfType(char const* name, int type) {
  EnsurePushMember(name);
  AssertIs(type);
}

void AssertIs(int type) {
  int actualType = lua_type(LuaState, -1);
  if (actualType != type) {
    std::string error =
        fmt::format("Unexpected type {}, expected {}", actualType, type);
    ImpLog(LogLevel::Fatal, LogChannel::Profile, "{:s}\n", error);
    throw std::runtime_error(error);
  }
}

void PushInitialIndex() { lua_pushnil(LuaState); }

int PushNextTableElement() { return lua_next(LuaState, -2); }

std::optional<Io::AssetPath> TryGetImpl<Io::AssetPath>::Call() {
  if (lua_isstring(LuaState, -1)) {
    return Io::AssetPath{.Mount = "", .FileName = lua_tostring(LuaState, -1)};
  }
  if (!lua_istable(LuaState, -1)) return std::nullopt;
  auto id = TryGetMember<uint32_t>("Id");
  auto mount = TryGetMember<std::string>("Mount");
  if (!id || !mount) return std::nullopt;
  return Io::AssetPath{.Mount = *mount, .Id = *id};
}

std::optional<RectF> TryGetImpl<RectF>::Call() {
  if (!lua_istable(LuaState, -1)) return std::nullopt;
  RectF outRectF;
  if (TryGetMember("X", outRectF.X) && TryGetMember("Y", outRectF.Y) &&
      TryGetMember("Width", outRectF.Width) &&
      TryGetMember("Height", outRectF.Height)) {
    return outRectF;
  }
  return std::nullopt;
}

std::optional<DialogueColorPair> TryGetImpl<DialogueColorPair>::Call() {
  if (!lua_istable(LuaState, -1)) return std::nullopt;
  DialogueColorPair outColor;
  if (TryGetMember("TextColor", outColor.TextColor) &&
      TryGetMember("OutlineColor", outColor.OutlineColor)) {
    return outColor;
  }

  const auto colorPair = TryGet<std::pair<uint32_t, uint32_t>>();
  if (colorPair.has_value()) {
    outColor.TextColor = colorPair->first;
    outColor.OutlineColor = colorPair->second;
    return outColor;
  }

  return std::nullopt;
}

std::optional<bool> TryGetImpl<bool>::Call() {
  if (lua_isboolean(LuaState, -1)) {
    return lua_toboolean(LuaState, -1);
  }
  // TODO conversion?

  return std::nullopt;
}

void ClearProfileInternal() {
  lua_close(LuaState);
  LuaState = nullptr;
}

}  // namespace Profile
}  // namespace Impacto