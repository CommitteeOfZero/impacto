#pragma once

#include "../impacto.h"
#include "../util.h"
#include "../animation.h"

namespace Impacto {
namespace UI {

class Widget {
 public:
  bool Enabled = false;
  bool HasFocus = false;
  bool Hovered = false;

  glm::vec4 Tint = glm::vec4(1.0f);

  virtual ~Widget();

  virtual void Update(float dt);
  virtual void UpdateInput() = 0;
  virtual void Render() = 0;

  // TODO: Text movement in widgets with text
  virtual void Move(glm::vec2 relativePosition, float duration);
  virtual void Move(glm::vec2 relativePosition);
  virtual void MoveTo(glm::vec2 pos, float duration);
  virtual void MoveTo(glm::vec2 pos);

  RectF Bounds = RectF(0.0f, 0.0f, 0.0f, 0.0f);

  glm::vec2 MoveTarget;
  glm::vec2 MoveOrigin;
  bool Moving = false;
  Animation MoveAnimation;
};

}  // namespace UI

}  // namespace Impacto