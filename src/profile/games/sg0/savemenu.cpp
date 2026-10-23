#include "savemenu.h"
#include "../../profile_internal.h"

#include "../../../game.h"
#include "../../../ui/ui.h"
#include "../../../games/sg0/savemenu.h"

namespace Impacto {
namespace Profile {
namespace SG0 {
namespace SaveMenu {

void Configure() {
  BackgroundSprite = EnsureGetMember<Sprite>("BackgroundSprite");
  PanelSprite = EnsureGetMember<Sprite>("PanelSprite");
  PanelPos = EnsureGetMember<glm::vec2>("PanelPos");
  GetMemberArray<Sprite>(std::span(HeaderSprites, 3), "HeaderSprites");
  HeaderPos = EnsureGetMember<glm::vec2>("HeaderPos");
  ButtonGuideSprite = EnsureGetMember<Sprite>("ButtonGuideSprite");
  ButtonGuidePos = EnsureGetMember<glm::vec2>("ButtonGuidePos");

  NoDataSprite = EnsureGetMember<Sprite>("NoDataSprite");
  EntryFrameSprite = EnsureGetMember<Sprite>("EntryFrameSprite");
  PlayTimeLabelSprite = EnsureGetMember<Sprite>("PlayTimeLabelSprite");
  SaveDateLabelSprite = EnsureGetMember<Sprite>("SaveDateLabelSprite");
  LockedSprite = EnsureGetMember<Sprite>("LockedSprite");
  GetMemberArray<Sprite>(std::span(PageDigitSprites, 7), "PageDigitSprites");

  EntryStart = EnsureGetMember<glm::vec2>("EntryStart");
  EntrySpacing = EnsureGetMember<glm::vec2>("EntrySpacing");
  FrameOffset = EnsureGetMember<glm::vec2>("FrameOffset");
  ThumbnailOffset = EnsureGetMember<glm::vec2>("ThumbnailOffset");
  ThumbnailSize = EnsureGetMember<glm::vec2>("ThumbnailSize");
  PlayTimeLabelOffset = EnsureGetMember<glm::vec2>("PlayTimeLabelOffset");
  SaveDateLabelOffset = EnsureGetMember<glm::vec2>("SaveDateLabelOffset");
  PlayTimeTextOffset = EnsureGetMember<glm::vec2>("PlayTimeTextOffset");
  SaveDateTextOffset = EnsureGetMember<glm::vec2>("SaveDateTextOffset");
  LockedOffset = EnsureGetMember<glm::vec2>("LockedOffset");
  PageNumberPos = EnsureGetMember<glm::vec2>("PageNumberPos");
  TextFontSize = EnsureGetMember<float>("TextFontSize");
  SubMenuCountMax = EnsureGetMember<int>("SubMenuCountMax");

  UI::SaveMenuPtr = new UI::SG0::SaveMenu();
  auto drawType = EnsureGetMember<Game::DrawComponentType>("DrawType");
  UI::Menus[drawType].push_back(UI::SaveMenuPtr);
}

}  // namespace SaveMenu
}  // namespace SG0
}  // namespace Profile
}  // namespace Impacto
