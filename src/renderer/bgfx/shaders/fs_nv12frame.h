#pragma once

#include "../uniform.h"

namespace Impacto::Bgfx {

template <>
struct Uniforms<FragmentShaderType::NV12Frame> {
  SamplerUniform<0> s_luma;
  SamplerUniform<1> s_cbCr;
  bool u_isAlpha = false;
};

}  // namespace Impacto::Bgfx
