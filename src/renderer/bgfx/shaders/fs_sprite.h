#pragma once

#include "../uniform.h"

namespace Impacto::Bgfx {

template <>
struct Uniforms<FragmentShaderType::Sprite> {
  SamplerUniform<0> s_texture;
  glm::vec3 u_colorShift = glm::vec3(0.0f);
};

}  // namespace Impacto::Bgfx
