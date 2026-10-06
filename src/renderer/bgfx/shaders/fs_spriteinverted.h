#pragma once

#include "../uniform.h"

namespace Impacto::Bgfx {

template <>
struct Uniforms<FragmentShaderType::SpriteInverted> {
  SamplerUniform<0> s_texture;
};

}  // namespace Impacto::Bgfx
