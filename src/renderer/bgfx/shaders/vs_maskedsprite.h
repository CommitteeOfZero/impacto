#pragma once

#include "../uniform.h"

namespace Impacto::Bgfx {

template <>
struct Uniforms<VertexShaderType::MaskedSprite> {
  glm::mat4 u_maskTransformation = glm::mat4(1.0f);
};

}  // namespace Impacto::Bgfx
