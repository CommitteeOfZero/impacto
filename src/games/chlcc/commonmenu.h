#pragma once

#include <vector>
#include "../../impacto.h"
#include "../../animation.h"
#include "../../spritesheet.h"

#include "animations/selectprompt.h"
#include "animations/menutransition.h"

namespace Impacto {
namespace UI {
namespace CHLCC {

class CommonMenu {
 public:
  CommonMenu(bool canGoFromSysMenu);

 protected:
  void DrawSubmenu(uint32_t backgroundColor, Sprite& CurrentCircleSprite,
                   Sprite& menuTitleSprite, float menuTitleAngle,
                   bool drawLeftTitle = false);
  void DrawSysMenu(glm::vec4 backgroundColor, Sprite& CurrentCircleSprite,
                   Sprite& menuTitleSprite, float menuTitleAngle);
  void DrawCircles(Sprite& CurrentCircleSprite) const;
  void DrawErin() const;
  void DrawRedBar() const;
  void DrawRightTitle(Sprite& titleSprite, float TitleAngle) const;
  void DrawLeftTitle(Sprite& titleSprite,
                     glm::vec4 tint = glm::vec4(1.0f)) const;
  void DrawBackgroundFilter();

  virtual void DrawButtonPrompt(
      const Sprite& buttonPromptSprite, glm::vec2 buttonPromptPos,
      std::optional<Animation> animation = std::nullopt);
  virtual void UpdateTitles(glm::vec2 rightTitlepos, glm::vec2 leftTitlePos,
                            bool vertical = true);

  Animation TitleFade;
  std::optional<Animation> FromSystemMenuTransition = std::nullopt;
  SelectPromptAnimation SelectAnimation;
  MenuTransitionAnimation MenuTransition;

  glm::vec2 RedTitleLabelPos;
  glm::vec2 RightTitlePos;
  glm::vec2 LeftTitlePos;
};

}  // namespace CHLCC
}  // namespace UI
}  // namespace Impacto