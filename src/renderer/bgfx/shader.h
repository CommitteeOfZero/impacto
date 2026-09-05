#pragma once

#include <bgfx/bgfx.h>
#include <SDL3/SDL_iostream.h>
#include <glm/gtc/type_ptr.hpp>

#include <cassert>
#include <string_view>
#include <optional>

#include "../../log.h"

#include "shaders/sprite.h"

namespace Impacto::Bgfx {

template <ShaderProgramType type>
class ShaderProgram {
 public:
  ShaderProgram(const ShaderProgram<type>&) = delete;
  ShaderProgram(ShaderProgram<type>&& other) { *this = std::move(other); }
  ~ShaderProgram() { Reset(true); }

  ShaderProgram<type>& operator=(const ShaderProgram<type>&) = delete;
  ShaderProgram<type>& operator=(ShaderProgram<type>&&);

  [[nodiscard]] static std::optional<ShaderProgram<type>> Create(
      const std::string_view vertexShaderName,
      const std::string_view fragmentShaderName);

  operator bgfx::ProgramHandle() { return GetProgramHandle(); }

  [[nodiscard]] bgfx::ProgramHandle GetProgramHandle() {
    assert(IsInit && bgfx::isValid(ProgramHandle));
    return ProgramHandle;
  }

  void SubmitUniforms(const Uniforms<type>& newUniforms) {
    ShaderUniformsState->Submit(newUniforms);
  }

 private:
  ShaderProgram() = default;

  bool IsInit = false;

  bgfx::ProgramHandle ProgramHandle;

  bgfx::ShaderHandle VertexShader;
  bgfx::ShaderHandle FragmentShader;

  void Reset(bool cleanUpResources);

  std::optional<UniformsState<type>> ShaderUniformsState;
};

template <ShaderProgramType type>
ShaderProgram<type>& ShaderProgram<type>::operator=(
    ShaderProgram<type>&& other) {
  if (&other == this) return *this;
  Reset(true);
  if (!other.IsInit) return *this;

  IsInit = true;
  ProgramHandle = other.ProgramHandle;
  VertexShader = other.VertexShader;
  FragmentShader = other.FragmentShader;

  other.Reset(false);

  return *this;
}

[[nodiscard]] static inline std::optional<bgfx::ShaderHandle> CreateShader(
    const char* const sourcePath) {
  size_t fileSize;
  void* const source = SDL_LoadFile(sourcePath, &fileSize);
  if (source == nullptr) {
    ImpLog(LogLevel::Error, LogChannel::Render,
           "Failed to read shader source file \"{:s}\": {:s}\n", sourcePath,
           SDL_GetError());
    return std::nullopt;
  }

  bgfx::ShaderHandle shader =
      bgfx::createShader(bgfx::copy(source, static_cast<uint32_t>(fileSize)));
  assert(bgfx::isValid(shader));

  SDL_free(source);

  return shader;
}

template <ShaderProgramType type>
std::optional<ShaderProgram<type>> ShaderProgram<type>::Create(
    const std::string_view vertexShaderName,
    const std::string_view fragmentShaderName) {
  const std::string_view pathPrefix = []() -> std::string_view {
    switch (bgfx::getRendererType()) {
      using enum bgfx::RendererType::Enum;
      case Direct3D11:
      case Direct3D12:
        return "dx11";
      case Metal:
        return "metal";
      case OpenGLES:
        return "essl";
      case OpenGL:
        return "glsl";
      case Vulkan:
        return "spirv";

      case Noop:
      case Agc:
      case Gnm:
      case Nvn:
      case Count:
        break;
    }
    return "";
  }();
  if (pathPrefix.empty()) {
    const std::string errorMsg =
        fmt::format("Unsupported renderer type for shaders: \"{:s}\"\n",
                    magic_enum::enum_name(bgfx::getRendererType()));
    ImpLog(LogLevel::Fatal, LogChannel::Render, "{:s}\n", errorMsg);
    throw errorMsg;
  }

  const std::string vertexShaderPath = fmt::format(
      "./shaders/bgfx/{:s}/{:s}.sc.bin", pathPrefix, vertexShaderName);
  const std::optional<const bgfx::ShaderHandle> vertexShader =
      CreateShader(vertexShaderPath.c_str());
  if (!vertexShader.has_value()) return std::nullopt;

  const std::string fragmentShaderPath = fmt::format(
      "./shaders/bgfx/{:s}/{:s}.sc.bin", pathPrefix, fragmentShaderName);
  const std::optional<const bgfx::ShaderHandle> fragmentShader =
      CreateShader(fragmentShaderPath.c_str());
  if (!fragmentShader.has_value()) return std::nullopt;

  ShaderProgram<type> program;

  program.VertexShader = *vertexShader;
  program.FragmentShader = *fragmentShader;

  program.ProgramHandle =
      bgfx::createProgram(program.VertexShader, program.FragmentShader, true);
  assert(bgfx::isValid(program.ProgramHandle));

  program.ShaderUniformsState.emplace(program.ProgramHandle);

  program.IsInit = true;

  return program;
}

template <ShaderProgramType type>
void ShaderProgram<type>::Reset(bool cleanUpResources) {
  if (!IsInit) return;
  IsInit = false;

  if (cleanUpResources) {
    bgfx::destroy(ProgramHandle);
  }
}

}  // namespace Impacto::Bgfx
