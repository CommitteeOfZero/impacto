#pragma once

#include "../uniform.h"

namespace Impacto::Bgfx {

template <>
struct Uniforms<FragmentShaderType::CCMessageBoxSprite> {
  SamplerUniform<0> s_texture;
  SamplerUniform<1> s_mask;
  glm::vec3 u_alpha;  // vec3(u_fadeRange, u_alphaVal, u_effectCt)
};

}  // namespace Impacto::Bgfx
