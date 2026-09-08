#pragma once

#include "../uniform.h"

namespace Impacto::Bgfx {

template <>
struct Uniforms<ShaderProgramType::Sprite> {
  glm::vec3 ColorShift = glm::vec3(0.0f);
};

template <>
struct UniformHandles<ShaderProgramType::Sprite> {
  DECLARE_UNIFORM_HANDLE(Sprite, ColorShift, u_colorShift, 1);
};

}  // namespace Impacto::Bgfx
