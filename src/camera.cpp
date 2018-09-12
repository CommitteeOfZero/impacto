#include "camera.h"

#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>

namespace Impacto {

Camera g_Camera;

void CameraMove(Camera* camera, glm::vec3 position) {
  glm::vec3 target = camera->Position + camera->Direction;
  camera->Position = position;
  camera->Direction = target - position;
}

void CameraLookAt(Camera* camera, glm::vec3 target) {
  camera->Direction = target - camera->Position;
}

void CameraResetTransform(Camera* camera) {
  camera->Position = glm::vec3(0.0f, 15.0f, -15.0f);
  camera->Up = glm::vec3(0.0f, 1.0f, 0.0f);
  CameraLookAt(camera, glm::vec3(0.0f, 7.0f, 2.0f));
}

void CameraResetPerspective(Camera* camera) {
  camera->Fov = 45.0f;
  camera->Near = 0.1f;
  camera->Far = 100.0f;
}

void CameraInit(Camera* camera) {
  CameraResetTransform(camera);
  CameraResetPerspective(camera);
  CameraRecalculate(camera);
}

void CameraRecalculate(Camera* camera) {
  camera->View = glm::lookAt(camera->Position,
                             camera->Position + camera->Direction, camera->Up);
  camera->Projection = glm::perspective(camera->Fov, camera->AspectRatio,
                                        camera->Near, camera->Far);
  camera->ViewProjection = camera->Projection * camera->View;
}

}  // namespace Impacto