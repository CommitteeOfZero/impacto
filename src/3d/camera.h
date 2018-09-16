#pragma once

#include <glm/glm.hpp>

namespace Impacto {

class Camera {
 public:
  // Move camera while keeping target
  void Move(glm::vec3 position);
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

  glm::vec3 Position;
  glm::vec3 Direction;
  glm::vec3 Up;

  // vertical FoV in degrees
  float Fov;
  float Near;
  float Far;

  glm::mat4 View;
  glm::mat4 Projection;
  glm::mat4 ViewProjection;
};

extern Camera g_Camera;

}  // namespace Impacto