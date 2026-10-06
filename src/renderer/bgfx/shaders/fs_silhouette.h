#pragma once

#include "../uniform.h"

namespace Impacto::Bgfx {

template <>
struct Uniforms<FragmentShaderType::Silhouette> {
  SamplerUniform<0> s_coverageMap;
};

}  // namespace Impacto::Bgfx
