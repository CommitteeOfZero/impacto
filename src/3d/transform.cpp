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

}  // namespace Impacto