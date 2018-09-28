#include "camera.h"

#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>

#include "../window.h"

namespace Impacto {

void Camera::Move(glm::vec3 position) {
  glm::vec3 target = Position + Direction;
  Position = position;
  Direction = target - position;
}

void Camera::LookAt(glm::vec3 target) { Direction = target - Position; }

void Camera::ResetTransform() {
  Position = glm::vec3(0.0f, 12.0f, 12.0f);
  Up = glm::vec3(0.0f, 1.0f, 0.0f);
  LookAt(glm::vec3(0.0f, 10.0f, 0.0f));
}

void Camera::ResetPerspective() {
  Fov = M_PI / 8.0f;
  Near = 0.1f;
  Far = 1000.0f;
}

void Camera::Init() {
  ResetTransform();
  ResetPerspective();
  Recalculate();
}

void Camera::Recalculate() {
  float aspectRatio = (float)g_WindowWidth / (float)g_WindowHeight;
  View = glm::lookAt(Position, Position + Direction, Up);
  Projection = glm::perspective(Fov, aspectRatio, Near, Far);
  ViewProjection = Projection * View;
}

}  // namespace Impacto