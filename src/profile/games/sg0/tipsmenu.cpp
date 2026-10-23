#include "tipsmenu.h"
#include "../../profile_internal.h"

#include "../../../game.h"
#include "../../../ui/ui.h"
#include "../../../games/sg0/tipsmenu.h"

namespace Impacto {
namespace Profile {
namespace SG0 {
namespace TipsMenu {

void Configure() {
  PanelsSprite = EnsureGetMember<Sprite>("PanelsSprite");
  HeaderSprite = EnsureGetMember<Sprite>("HeaderSprite");
  HeaderPos = EnsureGetMember<glm::vec2>("HeaderPos");
  HeaderIconSprite = EnsureGetMember<Sprite>("HeaderIconSprite");
  HeaderIconPos = EnsureGetMember<glm::vec2>("HeaderIconPos");
  GuideSprite = EnsureGetMember<Sprite>("GuideSprite");
  GuidePos = EnsureGetMember<glm::vec2>("GuidePos");

  GetMemberArray<Sprite>(std::span(TabSprites, TabCount), "TabSprites");
  GetMemberArray<Sprite>(std::span(TabDisabledSprites, TabCount),
                         "TabDisabledSprites");
  GetMemberArray<Sprite>(std::span(TabSelectedSprites, TabCount),
                         "TabSelectedSprites");
  GetMemberArray<RectF>(std::span(TabBounds, TabCount), "TabBounds");

  ListBounds = EnsureGetMember<RectF>("ListBounds");
  ListTop = EnsureGetMember<float>("ListTop");
  RowHeight = EnsureGetMember<float>("RowHeight");
  RowPadding = EnsureGetMember<float>("RowPadding");
  VisibleRows = EnsureGetMember<int>("VisibleRows");
  ListNumberX = EnsureGetMember<float>("ListNumberX");
  ListNameX = EnsureGetMember<float>("ListNameX");
  ListNameMaxWidth = EnsureGetMember<float>("ListNameMaxWidth");
  ListFontSize = EnsureGetMember<float>("ListFontSize");
  SelectionSprite = EnsureGetMember<Sprite>("SelectionSprite");
  NewBadgeSprite = EnsureGetMember<Sprite>("NewBadgeSprite");
  NewBadgeX = EnsureGetMember<float>("NewBadgeX");

  NumberPos = EnsureGetMember<glm::vec2>("NumberPos");
  CategoryPos = EnsureGetMember<glm::vec2>("CategoryPos");
  NamePos = EnsureGetMember<glm::vec2>("NamePos");
  NameMaxWidth = EnsureGetMember<float>("NameMaxWidth");
  HeaderFontSize = EnsureGetMember<float>("HeaderFontSize");
  NameFontSize = EnsureGetMember<float>("NameFontSize");
  NoteBounds = EnsureGetMember<RectF>("NoteBounds");
  NoteThumbSprite = EnsureGetMember<Sprite>("NoteThumbSprite");
  NoteThumbX = EnsureGetMember<float>("NoteThumbX");
  NoteScrollSpeed = EnsureGetMember<float>("NoteScrollSpeed");

  TextColor = EnsureGetMember<uint32_t>("TextColor");
  LockedColor = EnsureGetMember<uint32_t>("LockedColor");
  CountMax = EnsureGetMember<int>("CountMax");

  UI::TipsMenuPtr = new UI::SG0::TipsMenu();
  auto drawType = EnsureGetMember<Game::DrawComponentType>("DrawType");
  UI::Menus[drawType].push_back(UI::TipsMenuPtr);
}

}  // namespace TipsMenu
}  // namespace SG0
}  // namespace Profile
}  // namespace Impacto
