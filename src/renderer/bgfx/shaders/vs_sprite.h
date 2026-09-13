#pragma once

#include "../uniform.h"

namespace Impacto::Bgfx {

template <>
struct Uniforms<VertexShaderType::Sprite> {};

template <>
struct UniformHandles<VertexShaderType::Sprite> {};

}  // namespace Impacto::Bgfx
