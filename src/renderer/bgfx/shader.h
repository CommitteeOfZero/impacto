#pragma once

#include "bgfxhandle.h"

#include <bgfx/bgfx.h>
#include <bgfx/embedded_shader.h>
#include <SDL3/SDL_iostream.h>
#include <glm/gtc/type_ptr.hpp>
#include <magic_enum/magic_enum_containers.hpp>

#include <cassert>
#include <string_view>
#include <optional>

#include "../../log.h"

#include "shaders/vs_maskedsprite.h"
#include "shaders/vs_sprite.h"
#include "shaders/fs_additivemaskedsprite.h"
#include "shaders/fs_ccmessageboxsprite.h"
#include "shaders/fs_chlccmenubackground.h"
#include "shaders/fs_colorburnmaskedsprite.h"
#include "shaders/fs_colordodgemaskedsprite.h"
#include "shaders/fs_colormaskedsprite.h"
#include "shaders/fs_edgedetectedsinglesheetfont.h"
#include "shaders/fs_gaussianblur.h"
#include "shaders/fs_hardlightmaskedsprite.h"
#include "shaders/fs_linearburnmaskedsprite.h"
#include "shaders/fs_maskedsprite.h"
#include "shaders/fs_maskedspritebinary.h"
#include "shaders/fs_maskedspritenoalpha.h"
#include "shaders/fs_mosaic.h"
#include "shaders/fs_nv12frame.h"
#include "shaders/fs_overlaymaskedsprite.h"
#include "shaders/fs_screenmaskedsprite.h"
#include "shaders/fs_silhouette.h"
#include "shaders/fs_softlightmaskedsprite.h"
#include "shaders/fs_sprite.h"
#include "shaders/fs_spriteinverted.h"
#include "shaders/fs_yuvframe.h"

namespace Impacto::Bgfx {

class ShaderProgramInterface {
 public:
  [[nodiscard]] operator bgfx::ProgramHandle() { return GetProgramHandle(); }

  [[nodiscard]] virtual bgfx::ProgramHandle GetProgramHandle() = 0;
};

template <VertexShaderType vertexShaderType,
          FragmentShaderType fragmentShaderType>
class ShaderProgram final : public ShaderProgramInterface {
 public:
  ShaderProgram() = delete;
  ShaderProgram(const ShaderProgram<vertexShaderType, fragmentShaderType>&) =
      delete;
  ShaderProgram(ShaderProgram<vertexShaderType, fragmentShaderType>&&) =
      default;
  ~ShaderProgram() = default;

  ShaderProgram<vertexShaderType, fragmentShaderType>& operator=(
      const ShaderProgram<vertexShaderType, fragmentShaderType>&) = delete;
  ShaderProgram<vertexShaderType, fragmentShaderType>& operator=(
      ShaderProgram<vertexShaderType, fragmentShaderType>&&) = default;

  ShaderProgram(const bgfx::EmbeddedShader& vertexShader,
                const bgfx::EmbeddedShader& fragmentShader,
                std::function<void()> flush);

  [[nodiscard]] bgfx::ProgramHandle GetProgramHandle() override {
    assert(ProgramHandle.IsValid());
    return ProgramHandle;
  }

  void SubmitUniforms(const Uniforms<vertexShaderType>& newVertexUniforms,
                      const Uniforms<fragmentShaderType>& newFragmentUniforms) {
    VertexUniformsState->Submit(newVertexUniforms);
    FragmentUniformsState->Submit(newFragmentUniforms);
  }

 private:
  BgfxHandle<bgfx::ProgramHandle> ProgramHandle;

  BgfxHandle<bgfx::ShaderHandle> VertexShader;
  BgfxHandle<bgfx::ShaderHandle> FragmentShader;

  std::optional<UniformsState<vertexShaderType>> VertexUniformsState;
  std::optional<UniformsState<fragmentShaderType>> FragmentUniformsState;
};

template <VertexShaderType vertexShaderType,
          FragmentShaderType fragmentShaderType>
ShaderProgram<vertexShaderType, fragmentShaderType>::ShaderProgram(
    const bgfx::EmbeddedShader& vertexShader,
    const bgfx::EmbeddedShader& fragmentShader,
    const std::function<void()> flush) {
  VertexShader = bgfx::createEmbeddedShader(
      &vertexShader, bgfx::getRendererType(), vertexShader.name);
  assert(VertexShader.IsValid());

  FragmentShader = bgfx::createEmbeddedShader(
      &fragmentShader, bgfx::getRendererType(), fragmentShader.name);
  assert(FragmentShader.IsValid());

  ProgramHandle = bgfx::createProgram(VertexShader, FragmentShader, true);
  assert(ProgramHandle.IsValid());

  VertexUniformsState.emplace(VertexShader, flush);
  FragmentUniformsState.emplace(FragmentShader, flush);
}

}  // namespace Impacto::Bgfx
