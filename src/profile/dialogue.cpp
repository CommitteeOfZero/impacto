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

float FadeOutDuration;
float FadeInDuration;

float NVLBoxMaxOpacity;

TextAlignment ADVNameAlignment = TextAlignment::Left;
float ADVNameFontSize;
glm::vec2 ADVNamePos;

Sprite WaitIconSprite;
glm::vec2 WaitIconOffset;
float WaitIconAnimationDuration;

Font DialogueFont;
float DefaultFontSize;
float LineSpacing;
float RubyFontSize;
float RubyYOffset;

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
  EnsurePushMemberOfType("Dialogue", kObjectType);

  NVLBounds = EnsureGetMemberRectF("NVLBounds");
  ADVBounds = EnsureGetMemberRectF("ADVBounds");

  ADVBoxSprite = EnsureGetMemberSprite("ADVBoxSprite");
  ADVBoxPos = EnsureGetMemberVec2("ADVBoxPos");

  FadeOutDuration = EnsureGetMemberFloat("FadeOutDuration");
  FadeInDuration = EnsureGetMemberFloat("FadeInDuration");

  NVLBoxMaxOpacity = EnsureGetMemberFloat("NVLBoxMaxOpacity");

  ADVNameAlignment = TextAlignment::_from_integral_unchecked(
      EnsureGetMemberInt("ADVNameAlignment"));

  ADVNameFontSize = EnsureGetMemberFloat("ADVNameFontSize");
  ADVNamePos = EnsureGetMemberVec2("ADVNamePos");

  WaitIconSprite = EnsureGetMemberSprite("WaitIconSprite");
  WaitIconOffset = EnsureGetMemberVec2("WaitIconOffset");
  WaitIconAnimationDuration = EnsureGetMemberFloat("WaitIconAnimationDuration");

  DialogueFont = EnsureGetMemberFont("DialogueFont");
  DefaultFontSize = EnsureGetMemberFloat("DefaultFontSize");
  LineSpacing = EnsureGetMemberFloat("LineSpacing");
  RubyFontSize = EnsureGetMemberFloat("RubyFontSize");
  RubyYOffset = EnsureGetMemberFloat("RubyYOffset");

  MaxPageSize = EnsureGetMemberInt("MaxPageSize");
  PageCount = EnsureGetMemberInt("PageCount");

  Impacto::DialoguePages = new DialoguePage[PageCount];
  for (int i = 0; i < PageCount; i++) {
    Impacto::DialoguePages[i].Glyphs = new ProcessedTextGlyph[MaxPageSize];
  }

  {
    EnsurePushMemberOfType("ColorTable", kArrayType);

    auto const& _colors = TopVal();
    ColorCount = _colors.Size();
    ColorTable = new DialogueColorPair[ColorCount];
    for (uint32_t i = 0; i < ColorCount; i++) {
      PushArrayElement(i);
      AssertIs(kArrayType);
      auto const& _pair = TopVal();
      if (_pair.Size() != 2) {
        ImpLog(LL_Fatal, LC_Profile, "Expected two colors\n");
        Window::Shutdown();
      }
      ColorTable[i].TextColor = EnsureGetArrayElementUint(0);
      ColorTable[i].OutlineColor = EnsureGetArrayElementUint(1);
      Pop();
    }

    Pop();
  }

  HaveADVNameTag = TryPushMember("ADVNameTag");
  if (HaveADVNameTag) {
    AssertIs(kObjectType);

    ADVNameTag::Position = EnsureGetMemberVec2("Position");
    ADVNameTag::LeftSprite = EnsureGetMemberSprite("LeftSprite");
    ADVNameTag::LineSprite = EnsureGetMemberSprite("LineSprite");
    ADVNameTag::RightSprite = EnsureGetMemberSprite("RightSprite");
    ADVNameTag::BaseLineWidth = EnsureGetMemberFloat("BaseLineWidth");

    Pop();
  }

  Pop();
}

}  // namespace Dialogue
}  // namespace Profile
}  // namespace Impacto