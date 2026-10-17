#pragma once

#include "../uniform.h"

namespace Impacto::Bgfx {

template <>
struct Uniforms<FragmentShaderType::ColorMaskedSprite> {
  SamplerUniform<0> s_texture;
  SamplerUniform<1> s_mask;
};

}  // namespace Impacto::Bgfx
