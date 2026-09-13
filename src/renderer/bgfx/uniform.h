#pragma once

#include <glm/gtc/type_ptr.hpp>
#include <boost/pfr.hpp>
#include <bgfx/bgfx.h>

#include "../renderer.h"

inline bool operator!=(const bgfx::TextureHandle& lhs,
                       const bgfx::TextureHandle& rhs) {
  return lhs.idx != rhs.idx;
}

namespace Impacto::Bgfx {

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
struct UniformHandles;

template <ShaderType auto type>
class UniformsState {
 public:
  UniformsState() = delete;
  UniformsState(bgfx::ProgramHandle program, std::function<void()> flush)
      : Program(program), Flush(flush) {
    Submit(Uniforms<type>{});
  }

  void Submit(const Uniforms<type>& newUniforms) {
    constexpr static auto uniformNames =
        boost::pfr::names_as_array<Uniforms<type>>();
    constexpr static auto handleNames =
        boost::pfr::names_as_array<UniformHandles<type>>();
    static_assert(uniformNames.size() == handleNames.size());

    const auto updateUniform = [&]<std::size_t UniformIdx> {
      constexpr auto handleIt =
          std::ranges::find(handleNames, uniformNames[UniformIdx]);
      static_assert(handleIt != handleNames.end() &&
                    "Uniform and UniformHandles struct need to share the same "
                    "member names");
      constexpr size_t handleIdx = std::distance(handleNames.begin(), handleIt);

      if (!CurrentUniforms.has_value() ||
          boost::pfr::get<UniformIdx>(*CurrentUniforms) !=
              boost::pfr::get<UniformIdx>(newUniforms)) {
        if (CurrentUniforms.has_value()) Flush();

        SetUniform(boost::pfr::get<handleIdx>(Handles),
                   boost::pfr::get<UniformIdx>(newUniforms));
      }
    };

    [&]<std::size_t... Is>(std::index_sequence<Is...>) {
      (updateUniform.template operator()<Is>(), ...);
    }(std::make_index_sequence<uniformNames.size()>{});

    CurrentUniforms = newUniforms;
  }

 private:
  std::optional<Uniforms<type>> CurrentUniforms;
  UniformHandles<type> Handles;

  bgfx::ProgramHandle Program;

  std::function<void()> Flush;
};

struct UniformHandle {
 public:
  UniformHandle() = delete;
  UniformHandle(const UniformHandle&) = delete;
  UniformHandle(UniformHandle&& other) { *this = std::move(other); }
  virtual ~UniformHandle() { Reset(); }

  UniformHandle& operator=(const UniformHandle&) = delete;
  UniformHandle& operator=(UniformHandle&& other) {
    if (this == &other) return *this;
    Reset();

    Handle = other.Handle;
    other.Handle.idx = bgfx::kInvalidHandle;

    return *this;
  }

  UniformHandle(const char* name, bgfx::UniformType::Enum type,
                uint16_t num = 1)
      : Handle(bgfx::createUniform(name, type, num)) {
    assert(bgfx::isValid(Handle));
  }

  bgfx::UniformHandle GetHandle() {
    assert(bgfx::isValid(Handle));
    return Handle;
  }
  operator bgfx::UniformHandle() { return GetHandle(); }

 protected:
  bgfx::UniformHandle Handle = {bgfx::kInvalidHandle};

  void Reset() {
    if (bgfx::isValid(Handle)) bgfx::destroy(Handle);
    Handle.idx = bgfx::kInvalidHandle;
  }
};

struct SamplerHandle final : public UniformHandle {
 public:
  SamplerHandle(const char* name, uint8_t stage)
      : UniformHandle(name, bgfx::UniformType::Sampler, 1), Stage(stage) {}

  uint8_t GetStage() const { return Stage; }

 private:
  uint8_t Stage;
};

template <typename T>
constexpr bgfx::UniformType::Enum GetUniformType();

template <typename T>
  requires requires(T vec) {
    { glm::vec{vec} } -> std::same_as<T>;
  }
constexpr bgfx::UniformType::Enum GetUniformType() {
  return bgfx::UniformType::Vec4;
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
  bgfx::setTexture(0, handle, value);
}

#define DECLARE_UNIFORM_HANDLE(shaderType, memberName, uniformName) \
  UniformHandle memberName = UniformHandle(                         \
      #uniformName,                                                 \
      GetUniformType<decltype(Uniforms<shaderType>::memberName)>())

#define DECLARE_SAMPLER_HANDLE(shaderType, memberName, uniformName, stage) \
  static_assert(                                                           \
      std::is_same_v<bgfx::TextureHandle,                                  \
                     decltype(Uniforms<shaderType>::memberName)> &&        \
      "Sampler uniforms need to have uniform type bgfx::TextureHandle");   \
  SamplerHandle memberName = SamplerHandle(#uniformName, stage)

}  // namespace Impacto::Bgfx
