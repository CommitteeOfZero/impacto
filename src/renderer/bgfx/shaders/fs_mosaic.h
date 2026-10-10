#pragma once

#include "../uniform.h"

namespace Impacto::Bgfx {

template <>
struct Uniforms<FragmentShaderType::Mosaic> {
  SamplerUniform<0> s_texture;
  float u_tileSize;
};

}  // namespace Impacto::Bgfx
