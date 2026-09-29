#pragma once

#include "../uniform.h"

namespace Impacto::Bgfx {

template <>
struct Uniforms<FragmentShaderType::MaskedSpriteNoAlpha> {
  SamplerUniform<0> s_texture;
  SamplerUniform<1> s_mask;
  glm::vec2 u_alpha;
  bool u_isInverted;
};

}  // namespace Impacto::Bgfx
