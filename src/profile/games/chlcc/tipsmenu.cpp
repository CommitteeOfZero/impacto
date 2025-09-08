#include "tipsmenu.h"
#include "../../profile_internal.h"
#include "../../../log.h"
#include "../../../text.h"

#include "../../ui/tipsmenu.h"
#include "../../../game.h"
#include "../../../ui/ui.h"
#include "../../../games/chlcc/tipsmenu.h"

namespace Impacto {
namespace Profile {
namespace CHLCC {
namespace TipsMenu {

void Configure() {
  MenuTransitionDuration = EnsureGetMember<float>("TransitionDuration");
  BackgroundColor = EnsureGetMember<uint32_t>("BackgroundColor");
  CircleSprite = EnsureGetMember<Sprite>("CircleSprite");
  CircleStartPosition = EnsureGetMember<glm::vec2>("CircleStartPosition");
  CircleOffset = EnsureGetMember<float>("CircleOffset");
  ErinPosition = EnsureGetMember<glm::vec2>("ErinPosition");
  ErinSprite = EnsureGetMember<Sprite>("ErinSprite");
  BackgroundFilter = EnsureGetMember<Sprite>("BackgroundFilter");
  InitialRedBarPosition = EnsureGetMember<glm::vec2>("InitialRedBarPosition");
  RightRedBarPosition = EnsureGetMember<glm::vec2>("RightRedBarPosition");
  InitialRedBarSprite = EnsureGetMember<Sprite>("RedBarSprite");
  RedBarDivision = EnsureGetMember<float>("RedBarDivision");
  RedBarBaseX = EnsureGetMember<float>("RedBarBaseX");
  RedBarLabelPosition = EnsureGetMember<glm::vec2>("RedBarLabelPosition");
  RedBarLabel = EnsureGetMember<Sprite>("RedBarLabel");
  MenuTitleTextRightPosition =
      EnsureGetMember<glm::vec2>("MenuTitleTextRightPos");
  MenuTitleTextLeftPosition =
      EnsureGetMember<glm::vec2>("MenuTitleTextLeftPos");
  MenuTitleTextAngle = EnsureGetMember<float>("MenuTitleTextAngle");
  TitleFadeInDuration = EnsureGetMember<float>("TitleFadeInDuration");
  TitleFadeOutDuration = EnsureGetMember<float>("TitleFadeOutDuration");
  MenuTitleText = EnsureGetMember<Sprite>("MenuTitleText");
  TreePosition = EnsureGetMember<glm::vec2>("TreePosition");
  TipsTree = EnsureGetMember<Sprite>("TipsTree");
  GradientPosition = EnsureGetMember<glm::vec2>("GradientPosition");
  TipsGradient = EnsureGetMember<Sprite>("TipsGradient");
  EndOfGradientColor = EnsureGetMember<uint32_t>("EndOfGradientColor");
  CurrentTipBackgroundPosition =
      EnsureGetMember<glm::vec2>("CurrentTipBackgroundPosition");
  CurrentTipBackgroundSprite =
      EnsureGetMember<Sprite>("CurrentTipBackgroundSprite");
  ButtonPromptPosition = EnsureGetMember<glm::vec2>("ButtonPromptPosition");
  ButtonPromptSprite = EnsureGetMember<Sprite>("ButtonPromptSprite");

  TipsStringTable = EnsureGetMember<int>("TipsStringTable");
  CategoryStringIndex = EnsureGetMember<int>("CategoryStringIndex");
  SortStringIndex = EnsureGetMember<int>("SortStringIndex");
  PageSeparatorIndex = EnsureGetMember<int>("PageSeparatorIndex");
  LockedTipsIndex = EnsureGetMember<int>("LockedTipsIndex");
  NumberLabelStrIndex = EnsureGetMember<int>("NumberLabelStrIndex");
  NewLabelStrIndex = EnsureGetMember<int>("NewLabelStrIndex");
  UnreadLabelStrIndex = EnsureGetMember<int>("UnreadLabelStrIndex");
  auto str = EnsureGetMember<char const*>("CategoryString");
  TextGetSc3String(str, CategoryString);

  NumberLabelPosition = EnsureGetMember<glm::vec2>("NumberLabelPosition");
  NumberLabelFontSize = EnsureGetMember<float>("NumberLabelFontSize");
  NumberBounds = EnsureGetMember<RectF>("NumberBounds");
  NumberFontSize = EnsureGetMember<float>("NumberFontSize");
  CurrentPageBounds = EnsureGetMember<RectF>("CurrentPageBounds");
  TotalPagesBounds = EnsureGetMember<RectF>("TotalPagesBounds");
  DefaultColorIndex = EnsureGetMember<int>("DefaultColorIndex");
  UnreadColorIndex = EnsureGetMember<int>("UnreadColorIndex");

  PageSeparatorPosition = EnsureGetMember<glm::vec2>("PageSeparatorPosition");
  PageSeparatorFontSize = EnsureGetMember<float>("PageSeparatorFontSize");
  TipListEntryBounds = EnsureGetMember<RectF>("TipListEntryBounds");
  TipListEntryFontSize = EnsureGetMember<float>("TipListEntryFontSize");
  TipListYPadding = EnsureGetMember<float>("TipListYPadding");
  TipListEntryTextOffsetX = EnsureGetMember<float>("TipListEntryTextOffsetX");

  PronounciationFontSize = EnsureGetMember<float>("PronounciationFontSize");
  NameFontSize = EnsureGetMember<float>("NameFontSize");

  TipListEntryNameXOffset = EnsureGetMember<float>("TipListEntryNameXOffset");

  NameInitialBounds = EnsureGetMember<RectF>("NameInitialBounds");
  PronounciationInitialBounds =
      EnsureGetMember<RectF>("PronounciationInitialBounds");

  TipsListBounds = EnsureGetMember<RectF>("TipsListBounds");
  TipsListRenderBounds = EnsureGetMember<RectF>("TipsListRenderBounds");

  TipsEntryHighlightBar =
      EnsureGetMember<Sprite>("TipsEntryHighlightBarSprite");
  TipsEntryHighlightDot =
      EnsureGetMember<Sprite>("TipsEntryHighlightDotSprite");
  TipsEntryNewDot = EnsureGetMember<Sprite>("TipsEntryNewDotSprite");
  TipsLeftLine = EnsureGetMember<Sprite>("TipsLeftLineSprite");
  TipsLeftLineHole = EnsureGetMember<Sprite>("TipsLeftLineHoleSprite");
  TipsLeftLineEnd = EnsureGetMember<Sprite>("TipsLeftLineEndSprite");
  TipsLeftLineHoleEnd = EnsureGetMember<Sprite>("TipsLeftLineHoleEndSprite");
  TipsListBgBar = EnsureGetMember<Sprite>("TipsListBgBarSprite");
  TipsListBgBarHole = EnsureGetMember<Sprite>("TipsListBgBarHoleSprite");

  TipsListEntryDotOffset = EnsureGetMember<glm::vec2>("TipsListEntryDotOffset");
  TipsListNewDotOffset = EnsureGetMember<glm::vec2>("TipsListNewDotOffset");
  TipsScrollThumb = EnsureGetMember<Sprite>("TipsScrollThumbSprite");
  TipsScrollTrack = EnsureGetMember<Sprite>("TipsScrollTrackSprite");
  auto drawType = Game::DrawComponentType::_from_integral_unchecked(
      EnsureGetMember<uint8_t>("DrawType"));

  UI::TipsMenuPtr = new UI::CHLCC::TipsMenu();
  UI::Menus[drawType].push_back(UI::TipsMenuPtr);
}

}  // namespace TipsMenu
}  // namespace CHLCC
}  // namespace Profile
}  // namespace Impacto
