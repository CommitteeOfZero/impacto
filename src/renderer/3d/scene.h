#pragma once

#include <glm/glm.hpp>

#include "renderable3d.h"
#include "camera.h"

namespace Impacto {

class IScene3D {
 public:
  virtual void Init() = 0;
  virtual void Shutdown() = 0;
  virtual void Update(float dt) = 0;
  virtual void Render() = 0;

  Camera MainCamera;

  IRenderable3D *Renderables;

  glm::vec3 LightPosition;
  glm::vec4 Tint;
  bool DarkMode;
};

}  // namespace Impacto