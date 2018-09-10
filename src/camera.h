#pragma once

#include <glm/glm.hpp>

namespace Impacto {

struct Camera {
  glm::vec3 Position;
  glm::vec3 Direction;
  glm::vec3 Up;

  // vertical FoV in degrees
  float Fov;
  float Near;
  float Far;
  float AspectRatio;

  glm::mat4 View;
  glm::mat4 Projection;
  glm::mat4 ViewProjection;
};

extern Camera g_Camera;

// Move camera while keeping target
void CameraMove(Camera* camera, glm::vec3 position);
// Rotate camera to new target while keeping position
void CameraLookAt(Camera* camera, glm::vec3 target);
// Reset position, direction and up
void CameraResetTransform(Camera* camera);
// Reset FoV, near and far plane
void CameraResetPerspective(Camera* camera);
// Reset everything to defaults and recalculate matrices
void CameraInit(Camera* camera);
// Recalculate matrices
void CameraRecalculate(Camera* camera);

}  // namespace Impacto