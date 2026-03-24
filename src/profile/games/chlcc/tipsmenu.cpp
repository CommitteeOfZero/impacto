#include "tipsmenu.h"
#include "../../profile_internal.h"
#include "../../../log.h"
#include "../../../text/text.h"

#include "../../ui/tipsmenu.h"
#include "../../../game.h"
#include "../../../ui/ui.h"
#include "../../../games/chlcc/tipsmenu.h"

namespace Impacto {
namespace Profile {
namespace CHLCC {
namespace TipsMenu {

void Configure() {
  BackgroundColor = EnsureGetMember<uint32_t>("BackgroundColor");
  CircleSprite = EnsureGetMember<Sprite>("CircleSprite");
  MenuTitleTextRightPosition =
      EnsureGetMember<glm::vec2>("MenuTitleTextRightPos");
  MenuTitleTextLeftPosition =
      EnsureGetMember<glm::vec2>("MenuTitleTextLeftPos");
  MenuTitleTextAngle = EnsureGetMember<float>("MenuTitleTextAngle");
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
  {
    EnsurePushMember("CategoryString");
    auto str = EnsureGet<char const*>();
    TextGetSc3String(str, CategoryString);
    Pop();
  }
  NumberLabelPosition = EnsureGetMember<glm::vec2>("NumberLabelPosition");
  NumberLabelFontSize = EnsureGetMember<float>("NumberLabelFontSize");
  NumberBounds = EnsureGetMember<RectF>("NumberBounds");
  NumberFontSize = EnsureGetMember<float>("NumberFontSize");

  DefaultColorIndex = EnsureGetMember<int>("DefaultColorIndex");
  UnreadColorIndex = EnsureGetMember<int>("UnreadColorIndex");

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
  TipScrollbarPos = EnsureGetMember<glm::vec2>("TipScrollbarPos");
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

  CurrentPagePosition = EnsureGetMember<glm::vec2>("CurrentPagePosition");
  TotalPagesPosition = EnsureGetMember<glm::vec2>("TotalPagesPosition");
  PageSeparatorPosition = EnsureGetMember<glm::vec2>("PageSeparatorPosition");
  CurrentPageSprites =
      EnsureGetMember<std::vector<Sprite>>("CurrentPageSprites");
  TotalPageSprites = EnsureGetMember<std::vector<Sprite>>("TotalPageSprites");
  PageSeparatorSprite = EnsureGetMember<Sprite>("PageSeparatorSprite");

  SelectWordSprites = EnsureGetMember<std::vector<Sprite>>("SelectWordSprites");
  SelectWordPos = EnsureGetMember<std::vector<glm::vec2>>("SelectWordPos");
  if (SelectWordSprites.size() != SelectWordPos.size()) {
    throw std::runtime_error("Related arrays have mismatching sizes");
  }

  auto drawType = EnsureGetMember<Game::DrawComponentType>("DrawType");

  UI::TipsMenuPtr = new UI::CHLCC::TipsMenu();
  UI::Menus[drawType].push_back(UI::TipsMenuPtr);
}

}  // namespace TipsMenu
}  // namespace CHLCC
}  // namespace Profile
}  // namespace Impacto
