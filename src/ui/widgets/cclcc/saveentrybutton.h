#pragma once

#include <string>

#include "../button.h"
#include "../label.h"
#include "../../../renderer/renderer.h"
#include "../../../data/savesystem.h"

namespace Impacto {
namespace UI {
namespace Widgets {
namespace CCLCC {

class SaveEntryButton : public Widgets::Button {
 public:
  SaveEntryButton(int id, Sprite const& focusedBox, Sprite const& focusedText,
                  int page, glm::vec2 pos, Sprite lockedSymbol,
                  SaveSystem::SaveType saveType, Sprite noDataSprite,
                  Sprite brokenDataSprite);

  void Render() override;

  using Widget::Move;
  void Move(glm::vec2 pos) override;

  int GetPage() const;

  void AddThumbnail(Sprite thumbnail, glm::vec2 pos);
  void RefreshInfo();
  void ToggleLock();

  void AddNormalSpriteLabel(Sprite norm, glm::vec2 pos);
  void RefreshSaveDateText();
  void RefreshSceneTitleText(int strIndex);
  void RefreshCharacterRouteText(int strIndex);

 private:
  int Page;
  int SaveStatus;
  SaveSystem::SaveType Type;

  Label NormalSpriteLabel;
  Label FocusedSpriteLabel;
  Label LockedSymbol;
  Label Thumbnail;
  Label NoDataSymbol;
  Label BrokenDataSymbol;
  Label SaveDateLabel;
  Label SceneTitleLabel;
  Label CharacterRouteLabel;
};

}  // namespace CCLCC
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto