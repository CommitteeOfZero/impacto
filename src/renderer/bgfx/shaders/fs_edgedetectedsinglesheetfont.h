#pragma once

#include "../uniform.h"

namespace Impacto::Bgfx {

template <>
struct Uniforms<FragmentShaderType::EdgeDetectedSingleSheetFont> {
  SamplerUniform<0> s_font;
  SamplerUniform<1> s_mask;
  bool u_hasMask;
  glm::vec3 u_strength;  // u_differenceFactor, u_intensityShift, u_alphaShift
};

}  // namespace Impacto::Bgfx
