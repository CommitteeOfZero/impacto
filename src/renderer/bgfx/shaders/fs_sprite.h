#pragma once

#include "../uniform.h"

namespace Impacto::Bgfx {

template <>
struct Uniforms<FragmentShaderType::Sprite> {
  bgfx::TextureHandle s_texture = {bgfx::kInvalidHandle};
  glm::vec3 u_colorShift = glm::vec3(0.0f);
};

}  // namespace Impacto::Bgfx
