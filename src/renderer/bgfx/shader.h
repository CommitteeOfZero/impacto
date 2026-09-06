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

#include "shaders/sprite.h"

namespace Impacto::Bgfx {

template <ShaderProgramType type>
class ShaderProgram {
 public:
  ShaderProgram() = delete;
  ShaderProgram(const ShaderProgram<type>&) = delete;
  ShaderProgram(ShaderProgram<type>&& other) { *this = std::move(other); }
  ~ShaderProgram() { Reset(true); }

  ShaderProgram<type>& operator=(const ShaderProgram<type>&) = delete;
  ShaderProgram<type>& operator=(ShaderProgram<type>&&);

  ShaderProgram(const bgfx::EmbeddedShader& vertexShader,
                const bgfx::EmbeddedShader& fragmentShader);

  operator bgfx::ProgramHandle() { return GetProgramHandle(); }

  [[nodiscard]] bgfx::ProgramHandle GetProgramHandle() {
    assert(IsInit && bgfx::isValid(ProgramHandle));
    return ProgramHandle;
  }

  void SubmitUniforms(const Uniforms<type>& newUniforms) {
    ShaderUniformsState->Submit(newUniforms);
  }

 private:
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

template <ShaderProgramType type>
ShaderProgram<type>::ShaderProgram(const bgfx::EmbeddedShader& vertexShader,
                                   const bgfx::EmbeddedShader& fragmentShader) {
  VertexShader = bgfx::createEmbeddedShader(
      &vertexShader, bgfx::getRendererType(), vertexShader.name);
  assert(bgfx::isValid(VertexShader));

  FragmentShader = bgfx::createEmbeddedShader(
      &fragmentShader, bgfx::getRendererType(), fragmentShader.name);
  assert(bgfx::isValid(FragmentShader));

  ProgramHandle = bgfx::createProgram(VertexShader, FragmentShader, true);
  assert(bgfx::isValid(ProgramHandle));

  ShaderUniformsState.emplace(ProgramHandle);

  IsInit = true;
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
