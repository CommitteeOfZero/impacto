#pragma once

#include <glm/glm.hpp>

#include "transform.h"

namespace Impacto {

class Camera {
 public:
  // Rotate camera to new target while keeping position
  void LookAt(glm::vec3 target);
  // Reset position, direction and up
  void ResetTransform();
  // Reset FoV, near and far plane
  void ResetPerspective();
  // Reset everything to defaults and recalculate matrices
  void Init();
  // Recalculate matrices
  void Recalculate();

  Transform CameraTransform;
  glm::vec3 Up;

  // vertical FoV in radians
  float Fov;
  float Near;
  float Far;

  float AspectRatio;

  glm::mat4 View;
  glm::mat4 Projection;
  glm::mat4 ViewProjection;
};

}  // namespace Impacto