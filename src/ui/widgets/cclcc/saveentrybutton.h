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
  SaveEntryButton(int id, int index, Sprite const& focusedBox,
                  Sprite const& focusedText, int page, glm::vec2 pos,
                  Sprite lockedSymbol, SaveSystem::SaveType saveType,
                  Sprite noDataSprite, Sprite brokenDataSprite);

  void Render() override;
  void Move(glm::vec2 pos) override;
  void MoveTo(glm::vec2 pos) override;

  int GetPage() const;

  void AddThumbnail(Sprite thumbnail, glm::vec2 pos);
  void RefreshInfo();

  void AddNormalSpriteLabel(Sprite norm, glm::vec2 pos);
  void RefreshSaveDateText();
  void RefreshSceneTitleText(int strIndex);
  void RefreshCharacterRouteText(int strIndex);

 private:
  int Index;
  int Page;
  Label NormalSpriteLabel;
  Label FocusedSpriteLabel;
  Label LockedSymbol;
  Label Thumbnail;
  SaveSystem::SaveType Type;
  Label NoDataSymbol;
  Label BrokenDataSymbol;

  Label SaveDateLabel;
  Label SceneTitleLabel;
  Label CharacterRouteLabel;

  int SaveStatus;
  bool IsLocked;
};

}  // namespace CCLCC
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto