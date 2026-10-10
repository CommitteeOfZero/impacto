#pragma once

#include "../uniform.h"

namespace Impacto::Bgfx {

template <>
struct Uniforms<FragmentShaderType::GaussianBlur> {
  SamplerUniform<0> s_texture;
  bool u_isHorizontal;
};

}  // namespace Impacto::Bgfx
