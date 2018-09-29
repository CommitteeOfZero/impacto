#include "transform.h"

#include "../util.h"

namespace Impacto {

void Transform::SetRotationFromEuler(glm::vec3 eulerZYX) {
  eulerZYXToQuat(&eulerZYX, &Rotation);
}

glm::mat4 Transform::Matrix() {
  glm::mat4 result = glm::translate(glm::mat4(1.0), Position);
  result = result * glm::mat4_cast(Rotation);
  result = glm::scale(result, Scale);
  return result;
}

Transform Transform::Interpolate(Transform next, float factor) {
  Transform result;
  result.Position = glm::mix(Position, next.Position, factor);
  result.Rotation = glm::slerp(Rotation, next.Rotation, factor);
  result.Scale = glm::mix(Scale, next.Scale, factor);
  return result;
}

}  // namespace Impacto