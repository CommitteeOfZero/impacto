#pragma once

#include <glm/gtc/type_ptr.hpp>
#include <boost/pfr.hpp>
#include <bgfx/bgfx.h>
#include <set>

#include "../renderer.h"

inline bool operator!=(const bgfx::TextureHandle& lhs,
                       const bgfx::TextureHandle& rhs) {
  return lhs.idx != rhs.idx;
}

namespace Impacto::Bgfx {

template <typename T>
constexpr bgfx::UniformType::Enum GetUniformType();

template <typename T>
  requires requires(T vec) {
    { glm::vec{vec} } -> std::same_as<T>;
  }
constexpr bgfx::UniformType::Enum GetUniformType() {
  return bgfx::UniformType::Vec4;
}

template <>
constexpr bgfx::UniformType::Enum GetUniformType<bgfx::TextureHandle>() {
  return bgfx::UniformType::Sampler;
}

template <typename T>
void SetUniform(bgfx::UniformHandle handle, const T& value);

template <>
inline void SetUniform(bgfx::UniformHandle handle, const glm::vec4& value) {
  static_assert(GetUniformType<std::decay_t<decltype(value)>>() ==
                bgfx::UniformType::Vec4);

  bgfx::setUniform(handle, glm::value_ptr(value), 1);
}

template <typename T>
  requires(requires(T vec) {
    { glm::vec{vec} } -> std::same_as<T>;
  } && !std::is_same_v<glm::vec4, T>)
inline void SetUniform(bgfx::UniformHandle handle, const T& value) {
  static_assert(GetUniformType<std::decay_t<T>>() == bgfx::UniformType::Vec4);

  const glm::vec4 vec4(value, glm::vec<4 - T::length(), float>(0.0f));
  bgfx::setUniform(handle, glm::value_ptr(vec4), 1);
}

template <>
inline void SetUniform(bgfx::UniformHandle handle, const float& value) {
  SetUniform(handle, glm::vec1(value));
}

template <>
inline void SetUniform(bgfx::UniformHandle handle,
                       const bgfx::TextureHandle& value) {
  static_assert(GetUniformType<std::decay_t<decltype(value)>>() ==
                bgfx::UniformType::Sampler);

  bgfx::setTexture(0, handle, value);
}

enum class VertexShaderType {
  Sprite,
};
enum class FragmentShaderType {
  Sprite,
};

template <typename T>
concept ShaderType = std::is_same_v<T, VertexShaderType> ||
                     std::is_same_v<T, FragmentShaderType>;

template <ShaderType auto type>
struct Uniforms;

template <ShaderType auto type>
class UniformsState {
 public:
  UniformsState() = delete;
  UniformsState(bgfx::ShaderHandle shader, std::function<void()> flush)
      : Flush(flush) {
    const uint16_t internalUniformCount =
        bgfx::getShaderUniforms(shader, nullptr, 0);
    std::vector<bgfx::UniformHandle> unorderedHandles(internalUniformCount);
    bgfx::getShaderUniforms(shader, unorderedHandles.data(),
                            static_cast<uint16_t>(unorderedHandles.size()));

    // Some backends declare the same uniform multiple times, for some reason
    // Only count unique handles
    std::set<decltype(bgfx::UniformHandle::idx)> uniqueHandles;
    std::ranges::transform(unorderedHandles,
                           std::inserter(uniqueHandles, uniqueHandles.end()),
                           &bgfx::UniformHandle::idx);
    assert(uniqueHandles.size() == UniformCount &&
           "The number of uniforms defined in shader code is not equal to the "
           "number of uniforms defined in the struct. (Some types in the "
           "shader code may be illegal!)");

    constexpr static auto uniformNames =
        boost::pfr::names_as_array<Uniforms<type>>();
    [[maybe_unused]] constexpr static auto uniformTypes = [&]() {
      std::array<bgfx::UniformType::Enum, UniformCount> types;

      const auto getType = [&]<std::size_t UniformIdx> {
        types[UniformIdx] = GetUniformType<decltype(boost::pfr::get<UniformIdx>(
            Uniforms<type>{}))>();
      };

      [&]<std::size_t... Is>(std::index_sequence<Is...>) {
        (getType.template operator()<Is>(), ...);
      }(std::make_index_sequence<UniformCount>{});

      return types;
    }();

    for (decltype(bgfx::UniformHandle::idx) handle : uniqueHandles) {
      bgfx::UniformInfo info;
      bgfx::getUniformInfo({handle}, info);

      const auto uniformIt =
          std::ranges::find(uniformNames, std::string_view{info.name});
      assert(uniformIt != uniformNames.end() &&
             "Uniforms struct needs to share the same uniform names as "
             "the shader source code");
      const size_t uniformIdx = std::distance(uniformNames.begin(), uniformIt);

      assert(info.type == uniformTypes[uniformIdx] &&
             "A uniform's struct type cannot be converted to its shader type");

      Handles[uniformIdx] = {handle};
    }

    Submit(Uniforms<type>{});
  }

  void Submit(const Uniforms<type>& newUniforms) {
    const auto updateUniform = [&]<std::size_t UniformIdx> {
      if (CurrentUniforms.has_value() &&
          boost::pfr::get<UniformIdx>(*CurrentUniforms) !=
              boost::pfr::get<UniformIdx>(newUniforms)) {
        Flush();
      }
      SetUniform(Handles[UniformIdx], boost::pfr::get<UniformIdx>(newUniforms));
    };

    [&]<std::size_t... Is>(std::index_sequence<Is...>) {
      (updateUniform.template operator()<Is>(), ...);
    }(std::make_index_sequence<UniformCount>{});

    CurrentUniforms = newUniforms;
  }

 private:
  constexpr static size_t UniformCount =
      boost::pfr::names_as_array<Uniforms<type>>().size();

  std::optional<Uniforms<type>> CurrentUniforms;
  std::array<bgfx::UniformHandle, UniformCount> Handles;

  std::function<void()> Flush;
};

}  // namespace Impacto::Bgfx
