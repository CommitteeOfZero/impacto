#include "phone.h"
#include "../../profile_internal.h"

#include "../../../game.h"
#include "../../../ui/ui.h"
#include "../../../games/sg0/phone.h"

namespace Impacto {
namespace Profile {
namespace SG0 {
namespace Phone {

void Configure() {
  if (!TryPushMember("Phone")) return;
  AssertIs(LUA_TTABLE);

  BodySprite = EnsureGetMember<Sprite>("BodySprite");
  Scale = EnsureGetMember<float>("Scale");
  ShownPos = EnsureGetMember<glm::vec2>("ShownPos");
  HiddenX = EnsureGetMember<float>("HiddenX");
  ScreenOffset = EnsureGetMember<glm::vec2>("ScreenOffset");
  ScreenSize = EnsureGetMember<glm::vec2>("ScreenSize");
  WallpaperSprite = EnsureGetMember<Sprite>("WallpaperSprite");
  ScreenShadeAlpha = EnsureGetMember<float>("ScreenShadeAlpha");
  SelectionSprite = EnsureGetMember<Sprite>("SelectionSprite");
  Padding = EnsureGetMember<glm::vec2>("Padding");
  ItemsTop = EnsureGetMember<float>("ItemsTop");
  RowHeight = EnsureGetMember<float>("RowHeight");
  RowPadding = EnsureGetMember<float>("RowPadding");
  TitleFontSize = EnsureGetMember<float>("TitleFontSize");
  ItemFontSize = EnsureGetMember<float>("ItemFontSize");
  TextColor = EnsureGetMember<uint32_t>("TextColor");
  OutlineColor = EnsureGetMember<uint32_t>("OutlineColor");

  RineAvatarSprite = EnsureGetMember<Sprite>("RineAvatarSprite");
  RineAvatarStepY = EnsureGetMember<float>("RineAvatarStepY");
  RineStickerSprite = EnsureGetMember<Sprite>("RineStickerSprite");
  RineStickerStep = EnsureGetMember<glm::vec2>("RineStickerStep");
  RineStickerColumns = EnsureGetMember<int>("RineStickerColumns");
  RineTop = EnsureGetMember<float>("RineTop");
  RineMargin = EnsureGetMember<float>("RineMargin");
  RineGap = EnsureGetMember<float>("RineGap");
  RineAvatarSize = EnsureGetMember<float>("RineAvatarSize");
  RineStickerSize = EnsureGetMember<glm::vec2>("RineStickerSize");
  RineFontSize = EnsureGetMember<float>("RineFontSize");
  RineLineHeight = EnsureGetMember<float>("RineLineHeight");
  RineBubbleMaxWidth = EnsureGetMember<float>("RineBubbleMaxWidth");
  RineBubblePadding = EnsureGetMember<glm::vec2>("RineBubblePadding");
  RineOwnColor = EnsureGetMember<uint32_t>("RineOwnColor");
  RineOtherColor = EnsureGetMember<uint32_t>("RineOtherColor");
  RineTextColor = EnsureGetMember<uint32_t>("RineTextColor");
  RineReplyRowHeight = EnsureGetMember<float>("RineReplyRowHeight");
  RineReplyStickerSize = EnsureGetMember<glm::vec2>("RineReplyStickerSize");
  RineInputHeight = EnsureGetMember<float>("RineInputHeight");

  UI::SG0::PhonePtr = new UI::SG0::Phone();
  auto drawType = EnsureGetMember<Game::DrawComponentType>("DrawType");
  UI::Menus[drawType].push_back(UI::SG0::PhonePtr);

  Pop();
}

}  // namespace Phone
}  // namespace SG0
}  // namespace Profile
}  // namespace Impacto
