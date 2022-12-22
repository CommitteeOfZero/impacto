#include "camera.h"

#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>

#include "../../util.h"
#include "../../profile/scene3d.h"

namespace Impacto {

void Camera::LookAt(glm::vec3 target) {
  // for camera, need to invert this
  CameraTransform.SetRotationFromEuler(
      LookAtEulerZYX(target, CameraTransform.Position));
}

void Camera::ResetTransform() {
  CameraTransform.Position = Profile::Scene3D::DefaultCameraPosition;
  Up = glm::vec3(0.0f, 1.0f, 0.0f);
  LookAt(Profile::Scene3D::DefaultCameraTarget);
}

void Camera::ResetPerspective() {
  Fov = Profile::Scene3D::DefaultFov;
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
  // X axis is flipped in DaSH, for whatever reason
  if (Profile::Scene3D::Version == +LKMVersion::DaSH) {
    Transform xFlippedTransform = CameraTransform;
    xFlippedTransform.Scale.x = -xFlippedTransform.Scale.x;
    View = glm::inverse(xFlippedTransform.Matrix());
  } else {
    View = glm::inverse(
        CameraTransform.Matrix());  // move the world, not the camera
  }

  Projection = glm::perspective(Fov, AspectRatio, Near, Far);
  ViewProjection = Projection * View;
}

}  // namespace Impacto