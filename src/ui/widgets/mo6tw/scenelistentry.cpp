#include "scenelistentry.h"

#include "../../../renderer/renderer.h"

namespace Impacto {
namespace UI {
namespace Widgets {
namespace MO6TW {

SceneListEntry::SceneListEntry(int id, Widgets::Label* number,
                               Widgets::Label* lockedText,
                               Widgets::Label* unlockedText,
                               Sprite const& highlight, bool isLocked) {
  Id = id;
  Number = number;
  LockedText = lockedText;
  UnlockedText = unlockedText;
  HighlightSprite = highlight;
  IsLocked = isLocked;
  Enabled = true;
  Bounds =
      RectF(Number->Bounds.X, Number->Bounds.Y, 740.0f, Number->Bounds.Height);
}

void SceneListEntry::Render() {
  if (HasFocus) Renderer->DrawSprite(HighlightSprite, Bounds, Tint);
  Number->Render();
  if (IsLocked) {
    LockedText->Render();
  } else {
    UnlockedText->Render();
  }
}

void SceneListEntry::Move(glm::vec2 relativePosition) {
  Number->Move(relativePosition);
  LockedText->Move(relativePosition);
  UnlockedText->Move(relativePosition);

  Widget::Move(relativePosition);
}

}  // namespace MO6TW
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto