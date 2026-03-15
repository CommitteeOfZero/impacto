#pragma once

#include <minilua/minilua.h>
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
void ClearProfileInternal();

template <typename T>
std::optional<T> TryGet() {
  static_assert(sizeof(T*) == 0, "TryGet<T> not implemented for this type");
  return std::nullopt;
}

template <typename T>
bool TryGet(T& out);

template <typename T>
T EnsureGet();

template <typename T>
T EnsureGetKey();

template <typename T>
bool TryGetMember(char const* name, T& out);

template <typename T>
std::optional<T> TryGetMember(char const* name);

template <typename T>
T EnsureGetMember(char const* name);

template <typename T>
T EnsureGetArrayElement();
template <typename T>
T EnsureGetArrayElementByIndex(uint32_t index);

template <typename T>
void GetMemberArray(T* arr, size_t count, char const* name);

template <typename T>
std::vector<T> GetMemberVector(char const* name);

void ForEachProfileArray(std::invocable<uint32_t> auto func);

template <typename K>
void ForEachProfileTable(std::invocable<K const&> auto func);

template <typename T>
  requires is_any_of<std::decay_t<T>, char const*, std::string,
                     std::string_view>
std::optional<T> TryGet();

template <typename T>
  requires std::is_arithmetic_v<T>
std::optional<T> TryGet();

template <typename T>
concept HasEntityType =
    Impacto::is_any_of<T, Sprite, SpriteSheet, Font*, SpriteAnimationDef>;

template <typename T>
  requires HasEntityType<T>
std::optional<T> TryGet();

// generic glm::vec getter
template <typename T>
  requires requires(T x) {
    { glm::vec{x} } -> std::same_as<T>;
  }
std::optional<T> TryGet();

// generic map getter
template <typename T>
  requires std::is_same_v<
      typename T::value_type,
      std::pair<const typename T::key_type, typename T::mapped_type>>
std::optional<T> TryGet();

template <typename T>
  requires std::is_same_v<
      T, std::vector<typename T::value_type, typename T::allocator_type>>
std::optional<T> TryGet();
template <typename>
struct is_std_array : std::false_type {};
template <typename T, std::size_t N>
struct is_std_array<std::array<T, N>> : std::true_type {};

template <typename T>
  requires is_std_array<T>::value
std::optional<T> TryGet();

// Definitions:
template <typename T>
inline T EnsureGet() {
  std::optional<T> result = TryGet<T>();

  if (!result.has_value()) {
    ImpLog(Impacto::LogLevel::Fatal, Impacto::LogChannel::Profile,
           "Unexpected type\n");
    Game::Shutdown();
  }

  return result.value();
}

template <typename T>
inline T EnsureGetKey() {
  if constexpr (is_any_of<T, char const*, std::string_view, std::string>)
    return lua_tostring(LuaState, -2);
  if constexpr (std::is_integral_v<T> && !std::is_same_v<T, bool>)
    return (T)lua_tointeger(LuaState, -2);
}

template <typename T>
inline bool TryGetMember(char const* name, T& out) {
  if (!TryPushMember(name)) return false;
  bool result = TryGet<T>(out);
  Pop();
  return result;
}

template <typename T>
inline std::optional<T> TryGetMember(char const* name) {
  if (!TryPushMember(name)) return std::nullopt;
  auto result = TryGet<T>();
  Pop();
  return result;
}

template <typename T>
inline T EnsureGetMember(char const* name) {
  EnsurePushMember(name);
  T result = EnsureGet<T>();
  Pop();
  return result;
}

template <typename T>
inline T EnsureGetArrayElement() {
  return EnsureGet<T>();
}

template <typename T>
inline T EnsureGetArrayElementByIndex(uint32_t index) {
  lua_rawgeti(LuaState, -1, index + 1);
  T result = EnsureGet<T>();
  Pop();
  return result;
}

template <typename T>
inline bool TryGet(T& out) {
  std::optional<T> opt = TryGet<T>();
  if (opt) {
    out = std::move(*opt);
    return true;
  }
  return false;
}

template <typename T>
  requires is_any_of<std::decay_t<T>, char const*, std::string,
                     std::string_view>
inline std::optional<T> TryGet() {
  if (!lua_isstring(LuaState, -1)) return std::nullopt;
  return lua_tostring(LuaState, -1);
}

template <typename T>
  requires std::is_arithmetic_v<T>
inline std::optional<T> TryGet() {
  if (lua_isnumber(LuaState, -1)) {
    if constexpr (std::is_floating_point_v<T>) {
      return static_cast<T>(lua_tonumber(LuaState, -1));
    } else if constexpr (std::is_integral_v<T> && !is_any_of<T, bool>) {
      return static_cast<T>(lua_tointeger(LuaState, -1));
    }
  }
  if (lua_isstring(LuaState, -1)) {
    std::string_view inputStr = {lua_tostring(LuaState, -1)};
    T outNumber{};
    if constexpr (std::is_integral_v<T>) {
      auto [ptr, ec] = std::from_chars(
          inputStr.data(), inputStr.data() + inputStr.size(), outNumber);
      if (ec == std::errc{}) {
        return outNumber;
      }
      ImpLog(LogLevel::Fatal, LogChannel::Profile,
             "Error encountered converting {:s} to number: {:s}\n", inputStr,
             std::make_error_code(ec).message());
    } else {
      char* endPtr = nullptr;
      if constexpr (std::is_same_v<T, double>) {
        outNumber = std::strtod(inputStr.data(), &endPtr);
      } else if constexpr (std::is_same_v<T, float>) {
        outNumber = std::strtof(inputStr.data(), &endPtr);
      }
      if (endPtr != inputStr.data()) {
        return outNumber;
      }
      ImpLog(LogLevel::Fatal, LogChannel::Profile,
             "Error encountered converting {:s} to number: {:s}\n", inputStr,
             std::error_code{errno, std::generic_category()}.message());
    }
  }
  return std::nullopt;
}

template <typename T>
  requires HasEntityType<T>
inline const auto& GetEntityMap() {
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
inline std::optional<T> TryGet() {
  auto const& map = GetEntityMap<T>();
  std::optional<std::string_view> name = TryGet<std::string_view>();
  if (!name) return std::nullopt;
  const auto& ref = map.find(*name);
  if (ref == map.end()) return std::nullopt;

  return ref->second;
}

// generic glm::vec getter
template <typename T>
  requires requires(T x) {
    { glm::vec{x} } -> std::same_as<T>;
  }
inline std::optional<T> TryGet() {
  if (!lua_istable(LuaState, -1)) return std::nullopt;
  using value_type = typename T::value_type;
  std::array<std::optional<value_type>, T::length()> out;
  out[0] = TryGetMember<value_type>("X");
  if constexpr (T::length() > 1) out[1] = TryGetMember<value_type>("Y");
  if constexpr (T::length() > 2) out[2] = TryGetMember<value_type>("Z");
  if constexpr (T::length() > 3) out[3] = TryGetMember<value_type>("W");
  if (!std::ranges::all_of(out, [](auto& v) { return v.has_value(); }))
    return std::nullopt;

  return [&]<std::size_t... I>(std::index_sequence<I...>) {
    return std::optional<T>{T{*out[I]...}};
  }(std::make_index_sequence<T::length()>{});
}

// generic map getter
template <typename T>
  requires std::is_same_v<
      typename T::value_type,
      std::pair<const typename T::key_type, typename T::mapped_type>>
inline std::optional<T> TryGet() {
  if (!lua_istable(LuaState, -1)) return std::nullopt;

  T result;
  ForEachProfileTable<typename T::key_type>(
      [&](typename T::key_type const& key) {
        result.try_emplace(key, EnsureGet<typename T::mapped_type>());
      });
  return result;
}

template <typename T>
  requires std::is_same_v<
      T, std::vector<typename T::value_type, typename T::allocator_type>>
inline std::optional<T> TryGet() {
  if (!lua_istable(LuaState, -1)) return std::nullopt;

  T result;
  ForEachProfileArray([&]([[maybe_unused]] size_t i) {
    assert(i == result.size());
    result.push_back(EnsureGet<typename T::value_type>());
  });
  return result;
}

template <typename T>
  requires is_std_array<T>::value
inline std::optional<T> TryGet() {
  if (!lua_istable(LuaState, -1)) return std::nullopt;

  T result;
  size_t count = 0;
  ForEachProfileArray([&]([[maybe_unused]] size_t i) {
    assert(count == i);
    result[count++] = EnsureGet<typename T::value_type>();
  });
  assert(count == result.size());
  return result;
}

template <>
std::optional<Io::AssetPath> TryGet<Io::AssetPath>();
template <>
std::optional<RectF> TryGet<RectF>();
template <>
std::optional<bool> TryGet<bool>();
template <>
std::optional<DialogueColorPair> TryGet<DialogueColorPair>();

template <typename T>
inline void GetMemberArray(T* arr, size_t count, char const* name) {
  EnsurePushMemberOfType(name, LUA_TTABLE);

  size_t actualCount = static_cast<size_t>(lua_rawlen(LuaState, -1));
  if (actualCount != count) {
    ImpLog(LogLevel::Fatal, LogChannel::Profile,
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
inline std::vector<T> GetMemberVector(char const* name) {
  std::vector<T> result;
  EnsurePushMemberOfType(name, LUA_TTABLE);
  PushInitialIndex();
  while (PushNextTableElement()) {
    [[maybe_unused]] size_t i = EnsureGetKey<size_t>() - 1;
    assert(i == result.size());
    result.push_back(EnsureGetArrayElement<T>());
    Pop();
  }

  Pop();
  return result;
}

inline void ForEachProfileArray(std::invocable<uint32_t> auto func) {
  AssertIs(LUA_TTABLE);
  PushInitialIndex();
  while (PushNextTableElement() != 0) {
    auto index = EnsureGetKey<uint32_t>() - 1;
    func(index);
    Pop();
  }
}

template <typename K>
inline void ForEachProfileTable(std::invocable<K const&> auto func) {
  AssertIs(LUA_TTABLE);
  PushInitialIndex();
  while (PushNextTableElement() != 0) {
    auto key = EnsureGetKey<K>();
    func(key);
    Pop();
  }
}

}  // namespace Profile
}  // namespace Impacto