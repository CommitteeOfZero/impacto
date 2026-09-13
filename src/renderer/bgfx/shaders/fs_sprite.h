#pragma once

#include "../uniform.h"

namespace Impacto::Bgfx {

template <>
struct Uniforms<FragmentShaderType::Sprite> {
  bgfx::TextureHandle Texture = {bgfx::kInvalidHandle};
  glm::vec3 ColorShift = glm::vec3(0.0f);
};

template <>
struct UniformHandles<FragmentShaderType::Sprite> {
  DECLARE_SAMPLER_HANDLE(FragmentShaderType::Sprite, Texture, s_texture, 0);
  DECLARE_UNIFORM_HANDLE(FragmentShaderType::Sprite, ColorShift, u_colorShift);
};

}  // namespace Impacto::Bgfx
