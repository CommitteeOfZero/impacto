#pragma once

#include "../uniform.h"

namespace Impacto::Bgfx {

template <>
struct Uniforms<VertexShaderType::MaskedSprite> {
  glm::mat4 u_maskTransformation;
  bool u_fullscreenMask;
};

}  // namespace Impacto::Bgfx
