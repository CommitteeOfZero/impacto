#pragma once

#include <glm/gtc/quaternion.hpp>

namespace Impacto {

struct Transform {
  Transform() : Position(), Rotation(), Scale(1.0f) {}
  Transform(glm::vec3 position, glm::quat rotation, glm::vec3 scale)
      : Position(position), Rotation(rotation), Scale(scale) {}

  Transform(glm::vec3 position, glm::vec3 eulerZYX, glm::vec3 scale)
      : Position(position), Scale(scale) {
    SetRotationFromEuler(eulerZYX);
  }

  void SetRotationFromEuler(glm::vec3 eulerZYX);
  glm::mat4 Matrix();

  glm::vec3 Position;
  glm::quat Rotation;
  glm::vec3 Scale;
};

}  // namespace Impacto