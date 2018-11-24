#include "dialogue.h"
#include "profile_internal.h"
#include "../log.h"
#include "../window.h"

namespace Impacto {
namespace Profile {
namespace Dialogue {
RectF NVLBounds;
RectF ADVBounds;

Sprite ADVBoxSprite;
glm::vec2 ADVBoxPos;

TextAlignment ADVNameAlignment = TextAlignment::Left;
float ADVNameFontSize;
glm::vec2 ADVNamePos;

Sprite WaitIconSprite;

Font DialogueFont;
float DefaultFontSize;
float LineSpacing;
float RubyFontSize;

int ColorCount;
DialogueColorPair* ColorTable;

int MaxPageSize;
int PageCount;

bool HaveADVNameTag;
namespace ADVNameTag {
glm::vec2 Position;
Sprite LeftSprite;
Sprite LineSprite;
Sprite RightSprite;
float LineFactor;
float BaseLineWidth;
}  // namespace ADVNameTag

void Configure() {
  auto const& _dialogue =
      EnsureGetMemberOfType(Json, "/", "Dialogue", kObjectType);

  NVLBounds = EnsureGetMemberRectF(_dialogue, "/Dialogue", "NVLBounds");
  ADVBounds = EnsureGetMemberRectF(_dialogue, "/Dialogue", "ADVBounds");

  ADVBoxSprite = EnsureGetMemberSprite(_dialogue, "/Dialogue", "ADVBoxSprite");
  ADVBoxPos = EnsureGetMemberVec2(_dialogue, "/Dialogue", "ADVBoxPos");

  ADVNameAlignment = TextAlignment::_from_integral_unchecked(
      EnsureGetMemberInt(_dialogue, "/Dialogue", "ADVNameAlignment"));

  ADVNameFontSize =
      EnsureGetMemberFloat(_dialogue, "/Dialogue", "ADVNameFontSize");
  ADVNamePos = EnsureGetMemberVec2(_dialogue, "/Dialogue", "ADVNamePos");

  WaitIconSprite =
      EnsureGetMemberSprite(_dialogue, "/Dialogue", "WaitIconSprite");

  DialogueFont = EnsureGetMemberFont(_dialogue, "/Dialogue", "DialogueFont");
  DefaultFontSize =
      EnsureGetMemberFloat(_dialogue, "/Dialogue", "DefaultFontSize");
  LineSpacing = EnsureGetMemberFloat(_dialogue, "/Dialogue", "LineSpacing");
  RubyFontSize = EnsureGetMemberFloat(_dialogue, "/Dialogue", "RubyFontSize");

  MaxPageSize = EnsureGetMemberInt(_dialogue, "/Dialogue", "MaxPageSize");
  PageCount = EnsureGetMemberInt(_dialogue, "/Dialogue", "PageCount");

  Impacto::DialoguePages = new DialoguePage[PageCount];
  for (int i = 0; i < PageCount; i++) {
    Impacto::DialoguePages[i].Glyphs = new ProcessedTextGlyph[MaxPageSize];
  }

  auto const& _colors =
      EnsureGetMemberOfType(_dialogue, "/Dialogue", "ColorTable", kArrayType);

  ColorCount = _colors.Size();
  ColorTable = new DialogueColorPair[ColorCount];
  int i = 0;
  for (auto it = _colors.Begin(); it != _colors.End(); it++) {
    auto const& _pair = AssertIs(*it, "/Dialogue/ColorTable/x", kArrayType);
    if (_pair.Size() != 2) {
      ImpLog(LL_Fatal, LC_Profile, "Expected two colors\n");
      Window::Shutdown();
    }
    ColorTable[i].TextColor =
        EnsureGetUint(_pair[0], "/Dialogue/ColorTable/x/0");
    ColorTable[i].OutlineColor =
        EnsureGetUint(_pair[1], "/Dialogue/ColorTable/x/1");
    i++;
  }

  Value const& _advNameTag =
      TryGetMember(_dialogue, "ADVNameTag", HaveADVNameTag);
  if (HaveADVNameTag) {
    AssertIs(_advNameTag, "/Dialogue/ADVNameTag", kObjectType);

    ADVNameTag::Position =
        EnsureGetMemberVec2(_advNameTag, "/Dialogue/ADVNameTag", "Position");
    ADVNameTag::LeftSprite = EnsureGetMemberSprite(
        _advNameTag, "/Dialogue/ADVNameTag", "LeftSprite");
    ADVNameTag::LineSprite = EnsureGetMemberSprite(
        _advNameTag, "/Dialogue/ADVNameTag", "LineSprite");
    ADVNameTag::RightSprite = EnsureGetMemberSprite(
        _advNameTag, "/Dialogue/ADVNameTag", "RightSprite");
    ADVNameTag::LineFactor =
        EnsureGetMemberFloat(_advNameTag, "/Dialogue/ADVNameTag", "LineFactor");
    ADVNameTag::BaseLineWidth = EnsureGetMemberFloat(
        _advNameTag, "/Dialogue/ADVNameTag", "BaseLineWidth");
  }
}

}  // namespace Dialogue
}  // namespace Profile
}  // namespace Impacto