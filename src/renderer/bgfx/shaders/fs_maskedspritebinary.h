#pragma once

#include "../uniform.h"

namespace Impacto::Bgfx {

template <>
struct Uniforms<FragmentShaderType::MaskedSpriteBinary> {
  SamplerUniform<0> s_texture;
  SamplerUniform<1> s_mask;
  bool u_isInverted;
};

}  // namespace Impacto::Bgfx
