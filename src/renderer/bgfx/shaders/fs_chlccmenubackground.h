#pragma once

#include "../uniform.h"

namespace Impacto::Bgfx {

template <>
struct Uniforms<FragmentShaderType::CHLCCMenuBackground> {
  SamplerUniform<0> s_texture;
  SamplerUniform<1> s_mask;
  float u_alpha;
};

}  // namespace Impacto::Bgfx
