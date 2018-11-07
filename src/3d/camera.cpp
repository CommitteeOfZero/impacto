#include "camera.h"

#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>

#include "../window.h"

namespace Impacto {

void Camera::LookAt(glm::vec3 target) {
  // for camera, need to invert this
  CameraTransform.SetRotationFromEuler(
      LookAtEulerZYX(target, CameraTransform.Position));
}

void Camera::ResetTransform() {
  CameraTransform.Position = glm::vec3(0.0f, 12.5f, 23.0f);
  Up = glm::vec3(0.0f, 1.0f, 0.0f);
  LookAt(glm::vec3(0.0f, 12.5f, 0.0f));
}

void Camera::ResetPerspective() {
  Fov = M_PI / 8.0f;
  Near = 0.1f;
  Far = 1000.0f;
  AspectRatio = 1.0f;
}

void Camera::Init() {
  ResetTransform();
  ResetPerspective();
  Recalculate();
}

void Camera::Recalculate() {
  View =
      glm::inverse(CameraTransform.Matrix());  // move the world, not the camera
  Projection = glm::perspective(Fov, AspectRatio, Near, Far);
  ViewProjection = Projection * View;
}

}  // namespace Impacto