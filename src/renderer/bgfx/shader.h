#pragma once

#include <bgfx/bgfx.h>
#include <bgfx/embedded_shader.h>
#include <SDL3/SDL_iostream.h>
#include <glm/gtc/type_ptr.hpp>
#include <magic_enum/magic_enum_containers.hpp>

#include <cassert>
#include <string_view>
#include <optional>

#include "../../log.h"

#include "shaders/vs_sprite.h"
#include "shaders/fs_sprite.h"

namespace Impacto::Bgfx {

class ShaderProgramInterface {
 public:
  operator bgfx::ProgramHandle() { return GetProgramHandle(); }

  [[nodiscard]] virtual bgfx::ProgramHandle GetProgramHandle() = 0;
};

template <VertexShaderType vertexShaderType,
          FragmentShaderType fragmentShaderType>
class ShaderProgram final : public ShaderProgramInterface {
 public:
  ShaderProgram() = delete;
  ShaderProgram(const ShaderProgram<vertexShaderType, fragmentShaderType>&) =
      delete;
  ShaderProgram(ShaderProgram<vertexShaderType, fragmentShaderType>&& other) {
    *this = std::move(other);
  }
  ~ShaderProgram() { Reset(true); }

  ShaderProgram<vertexShaderType, fragmentShaderType>& operator=(
      const ShaderProgram<vertexShaderType, fragmentShaderType>&) = delete;
  ShaderProgram<vertexShaderType, fragmentShaderType>& operator=(
      ShaderProgram<vertexShaderType, fragmentShaderType>&&);

  ShaderProgram(const bgfx::EmbeddedShader& vertexShader,
                const bgfx::EmbeddedShader& fragmentShader,
                std::function<void()> flush);

  [[nodiscard]] bgfx::ProgramHandle GetProgramHandle() override {
    assert(bgfx::isValid(ProgramHandle));
    return ProgramHandle;
  }

  void SubmitUniforms(const Uniforms<vertexShaderType>& newVertexUniforms,
                      const Uniforms<fragmentShaderType>& newFragmentUniforms) {
    VertexUniformsState->Submit(newVertexUniforms);
    FragmentUniformsState->Submit(newFragmentUniforms);
  }

 private:
  bgfx::ProgramHandle ProgramHandle = {bgfx::kInvalidHandle};

  bgfx::ShaderHandle VertexShader = {bgfx::kInvalidHandle};
  bgfx::ShaderHandle FragmentShader = {bgfx::kInvalidHandle};

  void Reset(bool cleanUpResources);

  std::optional<UniformsState<vertexShaderType>> VertexUniformsState;
  std::optional<UniformsState<fragmentShaderType>> FragmentUniformsState;
};

template <VertexShaderType vertexShaderType,
          FragmentShaderType fragmentShaderType>
ShaderProgram<vertexShaderType, fragmentShaderType>&
ShaderProgram<vertexShaderType, fragmentShaderType>::operator=(
    ShaderProgram<vertexShaderType, fragmentShaderType>&& other) {
  if (&other == this) return *this;
  Reset(true);

  ProgramHandle = other.ProgramHandle;
  VertexShader = other.VertexShader;
  FragmentShader = other.FragmentShader;

  VertexUniformsState = std::move(other.VertexUniformsState);
  FragmentUniformsState = std::move(other.FragmentUniformsState);

  other.Reset(false);

  return *this;
}

template <VertexShaderType vertexShaderType,
          FragmentShaderType fragmentShaderType>
ShaderProgram<vertexShaderType, fragmentShaderType>::ShaderProgram(
    const bgfx::EmbeddedShader& vertexShader,
    const bgfx::EmbeddedShader& fragmentShader,
    const std::function<void()> flush) {
  VertexShader = bgfx::createEmbeddedShader(
      &vertexShader, bgfx::getRendererType(), vertexShader.name);
  assert(bgfx::isValid(VertexShader));

  FragmentShader = bgfx::createEmbeddedShader(
      &fragmentShader, bgfx::getRendererType(), fragmentShader.name);
  assert(bgfx::isValid(FragmentShader));

  ProgramHandle = bgfx::createProgram(VertexShader, FragmentShader, true);
  assert(bgfx::isValid(ProgramHandle));

  VertexUniformsState.emplace(VertexShader, flush);
  FragmentUniformsState.emplace(FragmentShader, flush);
}

template <VertexShaderType vertexShaderType,
          FragmentShaderType fragmentShaderType>
void ShaderProgram<vertexShaderType, fragmentShaderType>::Reset(
    bool cleanUpResources) {
  if (cleanUpResources) {
    if (bgfx::isValid(ProgramHandle)) bgfx::destroy(ProgramHandle);
  }

  ProgramHandle.idx = bgfx::kInvalidHandle;
  VertexShader.idx = bgfx::kInvalidHandle;
  FragmentShader.idx = bgfx::kInvalidHandle;

  VertexUniformsState.reset();
  FragmentUniformsState.reset();
}

}  // namespace Impacto::Bgfx
