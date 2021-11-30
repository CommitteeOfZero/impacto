#include "clearlistmenu.h"
#include "../../profile_internal.h"
#include "../../../ui/ui.h"
#include "../../../games/mo6tw/clearlistmenu.h"
#include "../../../text.h"

namespace Impacto {
namespace Profile {
namespace MO6TW {
namespace ClearListMenu {

Sprite BackgroundSprite;
Sprite WindowSprite;
glm::vec2 WindowPosition;

glm::vec2 LabelPosition;
Sprite ClearListLabel;
glm::vec2 EndingsLabelPosition;
Sprite EndingsLabel;
glm::vec2 ScenesLabelPosition;
Sprite ScenesLabel;
glm::vec2 CompletionLabelPosition;
Sprite CompletionLabel;
glm::vec2 AlbumLabelPosition;
Sprite AlbumLabel;
glm::vec2 PlayTimeLabelPosition;
Sprite PlayTimeLabel;
Sprite ClearListTextBackground;
glm::vec2 ClearListTextBGOffset;

Sprite EndingListLabel;
int EndingCount;
glm::vec2 EndingsListNumberInitialPosition;
glm::vec2 EndingsListTextInitialPosition;
glm::vec2 EndingsListTextMargin;
int EndingsListTextFontSize;
int EndingsListTextLockedTable;
int EndingsListTextLockedEntry;
int EndingsListTextTable;
int EndingsListTextColorIndex;

Sprite SceneTitleLabel;

float FadeInDuration;
float FadeOutDuration;

void Configure() {
  BackgroundSprite = EnsureGetMemberSprite("BackgroundSprite");
  WindowSprite = EnsureGetMemberSprite("WindowSprite");
  WindowPosition = EnsureGetMemberVec2("WindowPosition");

  LabelPosition = EnsureGetMemberVec2("LabelPosition");

  ClearListLabel = EnsureGetMemberSprite("ClearListLabel");
  EndingsLabelPosition = EnsureGetMemberVec2("EndingsLabelPosition");
  EndingsLabel = EnsureGetMemberSprite("EndingsLabel");
  ScenesLabelPosition = EnsureGetMemberVec2("ScenesLabelPosition");
  ScenesLabel = EnsureGetMemberSprite("ScenesLabel");
  CompletionLabelPosition = EnsureGetMemberVec2("CompletionLabelPosition");
  CompletionLabel = EnsureGetMemberSprite("CompletionLabel");
  AlbumLabelPosition = EnsureGetMemberVec2("AlbumLabelPosition");
  AlbumLabel = EnsureGetMemberSprite("AlbumLabel");
  PlayTimeLabelPosition = EnsureGetMemberVec2("PlayTimeLabelPosition");
  PlayTimeLabel = EnsureGetMemberSprite("PlayTimeLabel");
  ClearListTextBackground = EnsureGetMemberSprite("ClearListTextBackground");
  ClearListTextBGOffset = EnsureGetMemberVec2("ClearListTextBGOffset");

  EndingListLabel = EnsureGetMemberSprite("EndingListLabel");
  EndingCount = EnsureGetMemberInt("EndingCount");
  EndingsListNumberInitialPosition =
      EnsureGetMemberVec2("EndingsListNumberInitialPosition");
  EndingsListTextInitialPosition =
      EnsureGetMemberVec2("EndingsListTextInitialPosition");
  EndingsListTextMargin = EnsureGetMemberVec2("EndingsListTextMargin");
  EndingsListTextFontSize = EnsureGetMemberInt("EndingsListTextFontSize");
  EndingsListTextLockedTable = EnsureGetMemberInt("EndingsListTextLockedTable");
  EndingsListTextLockedEntry = EnsureGetMemberInt("EndingsListTextLockedEntry");
  EndingsListTextTable = EnsureGetMemberInt("EndingsListTextTable");
  EndingsListTextColorIndex = EnsureGetMemberInt("EndingsListTextColorIndex");

  SceneTitleLabel = EnsureGetMemberSprite("SceneTitleLabel");

  FadeInDuration = EnsureGetMemberFloat("FadeInDuration");
  FadeOutDuration = EnsureGetMemberFloat("FadeOutDuration");

  UI::ExtraMenus[UI::ExtraMenuCount++] = new UI::MO6TW::ClearListMenu();
}

}  // namespace ClearListMenu
}  // namespace MO6TW
}  // namespace Profile
}  // namespace Impacto