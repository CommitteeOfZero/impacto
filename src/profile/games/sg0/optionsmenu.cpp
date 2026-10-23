#include "optionsmenu.h"
#include "../../profile_internal.h"

#include "../../../game.h"
#include "../../../ui/ui.h"
#include "../../../games/sg0/optionsmenu.h"

namespace Impacto {
namespace Profile {
namespace SG0 {
namespace OptionsMenu {

void Configure() {
  BackgroundSprite = EnsureGetMember<Sprite>("BackgroundSprite");
  GuideSprite = EnsureGetMember<Sprite>("GuideSprite");
  GuidePos = EnsureGetMember<glm::vec2>("GuidePos");
  VoiceGuideSprite = EnsureGetMember<Sprite>("VoiceGuideSprite");
  VoiceGuidePos = EnsureGetMember<glm::vec2>("VoiceGuidePos");
  GetMemberArray<Sprite>(std::span(PageButtonSprites, PageCount),
                         "PageButtonSprites");
  GetMemberArray<RectF>(std::span(PageButtonBounds, PageCount),
                        "PageButtonBounds");
  GetMemberArray<Sprite>(std::span(PageHeadingSprites, PageCount),
                         "PageHeadingSprites");
  HeadingPos = EnsureGetMember<glm::vec2>("HeadingPos");

  ContentX = EnsureGetMember<float>("ContentX");
  RowsTop = EnsureGetMember<float>("RowsTop");
  RowHeight = EnsureGetMember<float>("RowHeight");
  VoiceRowHeight = EnsureGetMember<float>("VoiceRowHeight");
  RowPadding = EnsureGetMember<float>("RowPadding");
  RowWidth = EnsureGetMember<float>("RowWidth");

  SliderFrameSprite = EnsureGetMember<Sprite>("SliderFrameSprite");
  SliderFillSprite = EnsureGetMember<Sprite>("SliderFillSprite");
  SliderKnobSprite = EnsureGetMember<Sprite>("SliderKnobSprite");
  SliderOffset = EnsureGetMember<glm::vec2>("SliderOffset");
  SliderFillOffset = EnsureGetMember<glm::vec2>("SliderFillOffset");
  SliderKnobOffsetY = EnsureGetMember<float>("SliderKnobOffsetY");
  VoiceSliderScale = EnsureGetMember<float>("VoiceSliderScale");
  VoiceSliderOffsetY = EnsureGetMember<float>("VoiceSliderOffsetY");
  VoiceOffSprite = EnsureGetMember<Sprite>("VoiceOffSprite");

  const uint32_t color = EnsureGetMember<uint32_t>("HighlightColor");
  HighlightColor =
      glm::vec3((color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF) /
      255.0f;
  HighlightOpacity = EnsureGetMember<float>("HighlightOpacity");
  UnselectedBrightness = EnsureGetMember<float>("UnselectedBrightness");
  CountMax = EnsureGetMember<int>("CountMax");

  Rows.clear();
  EnsurePushMemberOfType("Rows", LUA_TTABLE);
  ForEachProfileArray([](uint32_t index) {
    Row row;
    row.Page = EnsureGetMember<int>("Page");
    const std::string setting = EnsureGetMember<std::string>("Setting");
    const auto kind = magic_enum::enum_cast<Setting>(setting);
    if (!kind) {
      throw std::runtime_error(
          fmt::format("Unknown option setting {}", setting));
    }
    row.Kind = *kind;
    row.Index = TryGetMember<int>("Index").value_or(0);
    row.Label = EnsureGetMember<Sprite>("Label");
    row.Choices = TryGetMember<std::vector<Sprite>>("Choices").value_or(
        std::vector<Sprite>{});
    if (Rows.size() <= index) Rows.resize(index + 1);
    Rows[index] = std::move(row);
  });
  Pop();

  UI::OptionsMenuPtr = new UI::SG0::OptionsMenu();
  auto drawType = EnsureGetMember<Game::DrawComponentType>("DrawType");
  UI::Menus[drawType].push_back(UI::OptionsMenuPtr);
}

}  // namespace OptionsMenu
}  // namespace SG0
}  // namespace Profile
}  // namespace Impacto
