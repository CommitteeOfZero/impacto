#pragma once

#include "../uniform.h"

namespace Impacto::Bgfx {

template <>
struct Uniforms<FragmentShaderType::YUVFrame> {
  SamplerUniform<0> s_luma;
  SamplerUniform<1> s_cb;
  SamplerUniform<2> s_cr;
  bool u_isAlpha = false;
};

}  // namespace Impacto::Bgfx
