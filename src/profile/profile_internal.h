#pragma once

#include "../minilua_impl.h"
#include <initializer_list>
#include <optional>
#include <charconv>
#include <system_error>
#include <glm/glm.hpp>
#include "../io/assetpath.h"
#include "../util.h"
#include "../log.h"
#include "../game.h"

#include "animations.h"
#include "sprites.h"
#include "fonts.h"
#include "../spritesheet.h"
#include "../font.h"
#include "../spriteanimation.h"

namespace Impacto {
namespace Profile {

inline lua_State* LuaState;

void LuaDumpStack();
void Pop();
bool TryPushMember(char const* name);
void EnsurePushMember(char const* name);
void EnsurePushMemberOfType(char const* name, int type);
void AssertIs(int type);
void PushInitialIndex();
int PushNextTableElement();

template <typename T>
bool TryGet(T& out) {
  static_assert(sizeof(T*) == 0, "Invalid Type");
}

template <typename T>
std::optional<T> TryGet();

template <typename T>
  requires std::is_arithmetic_v<T>
bool TryGet(T& outNumber) {
  if (lua_isnumber(LuaState, -1)) {
    if constexpr (std::is_floating_point_v<T>) {
      outNumber = static_cast<T>(lua_tonumber(LuaState, -1));
    } else if constexpr (std::is_integral_v<T> && !is_any_of<T, bool>) {
      outNumber = static_cast<T>(lua_tointeger(LuaState, -1));
    }
    return true;
  }
  if (lua_isstring(LuaState, -1)) {
    std::string_view inputStr = {lua_tostring(LuaState, -1)};
    if constexpr (std::is_integral_v<T>) {
      auto [ptr, ec] = std::from_chars(
          inputStr.data(), inputStr.data() + inputStr.size(), outNumber);
      if (ec != std::errc{}) {
        ImpLog(LL_Fatal, LC_Profile,
               "Error encountered converting {:s} to number: {:s}\n", inputStr,
               std::make_error_code(ec).message());
        return false;
      }
      return true;
    } else {
      char* endPtr = nullptr;
      if constexpr (std::is_same_v<T, double>) {
        outNumber = std::strtod(inputStr.data(), &endPtr);
      } else if constexpr (std::is_same_v<T, float>) {
        outNumber = std::strtof(inputStr.data(), &endPtr);
      }
      if (endPtr == inputStr.data()) {
        ImpLog(LL_Fatal, LC_Profile,
               "Error encountered converting {:s} to number: {:s}\n", inputStr,
               std::error_code{errno, std::generic_category()}.message());
        return false;
      }
      return true;
    }
  }
  return false;
}

template <typename T>
concept HasEntityType =
    Impacto::is_any_of<T, Sprite, SpriteSheet, Font*, SpriteAnimationDef>;

template <typename T>
  requires HasEntityType<T>
const auto& GetEntityMap() {
  using decayedT = std::decay_t<T>;
  using namespace std::literals::string_view_literals;
  if constexpr (std::is_same_v<decayedT, Sprite>) {
    return Sprites;
  } else if constexpr (std::is_same_v<decayedT, SpriteSheet>) {
    return SpriteSheets;
  } else if constexpr (std::is_same_v<decayedT, Font*>) {
    return Fonts;
  } else if constexpr (std::is_same_v<decayedT, SpriteAnimationDef>) {
    return Animations;
  } else {
    static_assert(sizeof(T*) == 0, "Not a valid Entity type");
  }
}

template <typename T>
  requires HasEntityType<T>
bool TryGet(T& out) {
  auto const& map = GetEntityMap<T>();
  std::optional<std::string_view> name = TryGet<std::string_view>();
  if (!name) return false;
  const auto& ref = map.find(*name);
  if (ref == map.end()) return false;

  out = ref->second;
  return true;
}
template <typename T>
  requires is_any_of<std::decay_t<T>, char const*, std::string,
                     std::string_view>
bool TryGet(T& outString) {
  if (!lua_isstring(LuaState, -1)) return false;
  outString = lua_tostring(LuaState, -1);
  return true;
}

template <>
bool TryGet<Io::AssetPath>(Io::AssetPath& outPath);
template <>
bool TryGet<RectF>(RectF& outRectF);
template <>
bool TryGet<glm::vec3>(glm::vec3& outVec3);
template <>
bool TryGet<glm::vec2>(glm::vec2& outVec2);
template <>
bool TryGet<bool>(bool& outBool);

template <typename T>
T EnsureGetKey() {
  if constexpr (is_any_of<T, char const*, std::string_view, std::string>)
    return lua_tostring(LuaState, -2);
  if constexpr (std::is_integral_v<T> && !std::is_same_v<T, bool>)
    return lua_tointeger(LuaState, -2);
}

void ClearProfileInternal();

template <typename T>
T EnsureGet() {
  T result;
  bool success = TryGet(result);
  if (!success) {
    ImpLog(Impacto::LL_Fatal, Impacto::LC_Profile, "Unexpected type\n");
    Game::Shutdown();
  }
  return result;
}

template <typename T>
inline std::optional<T> TryGet() {
  if (T v; TryGet<T>(v)) {
    return std::optional<T>{std::in_place, std::move(v)};
  }
  return std::nullopt;
}

template <typename T>
bool TryGetMember(char const* name, T& out) {
  if (!TryPushMember(name)) return false;
  bool result = TryGet<T>(out);
  Pop();
  return result;
}

template <typename T>
std::optional<T> TryGetMember(char const* name) {
  if (!TryPushMember(name)) return std::nullopt;
  auto result = TryGet<T>();
  Pop();
  return result;
}

template <typename T>
T EnsureGetMember(char const* name) {
  EnsurePushMember(name);
  T result = EnsureGet<T>();
  Pop();
  return result;
}

template <typename T>
bool TryGetArrayElement(T& out) {
  return TryGet<T>(out);
}

template <typename T>
std::optional<T> TryGetArrayElement() {
  return TryGet<T>();
}

template <typename T>
T EnsureGetArrayElement() {
  return EnsureGet<T>();
}

template <typename T>
T EnsureGetArrayElementByIndex(uint32_t index) {
  lua_rawgeti(LuaState, -1, index + 1);
  T result = EnsureGet<T>();
  Pop();
  return result;
}

template <typename T>
void GetMemberArray(T* arr, uint32_t count, char const* name) {
  EnsurePushMemberOfType(name, LUA_TTABLE);

  uint32_t actualCount = static_cast<uint32_t>(lua_rawlen(LuaState, -1));
  if (actualCount != count) {
    ImpLog(LL_Fatal, LC_Profile,
           "Expected to have {:d} values for {:s}, got {:d}\n", count, name,
           actualCount);
    Window->Shutdown();
  }

  PushInitialIndex();
  while (PushNextTableElement()) {
    size_t i = EnsureGetKey<size_t>() - 1;
    arr[i] = EnsureGetArrayElement<T>();
    Pop();
  }
  Pop();
}

template <typename T>
std::vector<T> GetMemberVector(char const* name) {
  std::vector<T> result;
  EnsurePushMemberOfType(name, LUA_TTABLE);
  PushInitialIndex();
  while (PushNextTableElement()) {
    size_t i = EnsureGetKey<size_t>() - 1;
    assert(i == result.size());
    result.push_back(EnsureGetArrayElement<T>());
    Pop();
  }

  Pop();
  return result;
}

}  // namespace Profile
}  // namespace Impacto