#include "dialogue.h"
#include "profile_internal.h"
#include "../log.h"
#include "../ui/ui.h"

#include "games/mo6tw/dialoguebox.h"
#include "games/chlcc/dialoguebox.h"
#include "games/cc/dialoguebox.h"

namespace Impacto {
namespace Profile {
namespace Dialogue {
RectF NVLBounds;
RectF ADVBounds;
RectF REVBounds;
RectF TipsBounds;

Sprite ADVBoxSprite;
glm::vec2 ADVBoxPos;

float FadeOutDuration;
float FadeInDuration;

DialogueBoxType DialogueBoxCurrentType{DialogueBoxType::None};

float NVLBoxMaxOpacity;

TextAlignment ADVNameAlignment = TextAlignment::Left;
float ADVNameFontSize;
glm::vec2 ADVNamePos;
bool ADVBoxShowName = true;

float REVNameFontSize;
int REVNameColor;
float REVNameOffset;

float TipsLineSpacing;

Sprite WaitIconSprite;
SpriteAnimationDef WaitIconSpriteAnim;
glm::vec2 WaitIconOffset;
float WaitIconAnimationDuration;
Impacto::WaitIconDisplay::WaitIconType WaitIconCurrentType{
    Impacto::WaitIconDisplay::WaitIconType::None};

Font* DialogueFont;
float DefaultFontSize;
float RubyFontSize;
float RubyYOffset;

int ColorCount;
DialogueColorPair* ColorTable;

int MaxPageSize;
int PageCount;

bool ColorTagIsUint8;

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
  REVBounds = EnsureGetMemberRectF("REVBounds");
  TryGetMemberRectF("TipsBounds", TipsBounds);

  ADVBoxSprite = EnsureGetMemberSprite("ADVBoxSprite");
  ADVBoxPos = EnsureGetMemberVec2("ADVBoxPos");

  TryGetMemberBool("ADVBoxShowName", ADVBoxShowName);

  FadeOutDuration = EnsureGetMemberFloat("FadeOutDuration");
  FadeInDuration = EnsureGetMemberFloat("FadeInDuration");

  DialogueBoxCurrentType = DialogueBoxType::_from_integral_unchecked(
      EnsureGetMemberInt("DialogueBoxCurrentType"));

  switch (DialogueBoxCurrentType) {
    case DialogueBoxType::MO6TW:
      Profile::MO6TW::DialogueBox::Configure();
      break;
    case DialogueBoxType::CHLCC:
      Profile::CHLCC::DialogueBox::Configure();
      break;
    case DialogueBoxType::CC:
      Profile::CC::DialogueBox::Configure();
      break;
  }

  NVLBoxMaxOpacity = EnsureGetMemberFloat("NVLBoxMaxOpacity");

  ADVNameAlignment = TextAlignment::_from_integral_unchecked(
      EnsureGetMemberInt("ADVNameAlignment"));

  ADVNameFontSize = EnsureGetMemberFloat("ADVNameFontSize");
  ADVNamePos = EnsureGetMemberVec2("ADVNamePos");

  REVNameFontSize = EnsureGetMemberFloat("REVNameFontSize");
  REVNameColor = EnsureGetMemberInt("REVNameColor");
  REVNameOffset = EnsureGetMemberFloat("REVNameOffset");

  TryGetMemberFloat("TipsLineSpacing", TipsLineSpacing);

  WaitIconCurrentType = WaitIconDisplay::WaitIconType::_from_integral_unchecked(
      EnsureGetMemberInt("WaitIconCurrentType"));
  if (WaitIconCurrentType == +WaitIconDisplay::WaitIconType::SpriteAnim) {
    WaitIconSpriteAnim = EnsureGetMemberAnimation("WaitIconSpriteAnim");
  } else if (WaitIconCurrentType ==
             +WaitIconDisplay::WaitIconType::SpriteAnimFixed) {
    WaitIconSprite = EnsureGetMemberSprite("WaitIconSprite");
    WaitIconSpriteAnim = EnsureGetMemberAnimation("WaitIconSpriteAnim");
  } else {
    WaitIconSprite = EnsureGetMemberSprite("WaitIconSprite");
    WaitIconAnimationDuration =
        EnsureGetMemberFloat("WaitIconAnimationDuration");
  }
  WaitIconOffset = EnsureGetMemberVec2("WaitIconOffset");

  DialogueFont = EnsureGetMemberFont("DialogueFont");
  DefaultFontSize = EnsureGetMemberFloat("DefaultFontSize");
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
        Window->Shutdown();
      }
      ColorTable[i].TextColor = EnsureGetArrayElementUint(0);
      ColorTable[i].OutlineColor = EnsureGetArrayElementUint(1);
      Pop();
    }

    Pop();
  }

  ColorTagIsUint8 = EnsureGetMemberBool("ColorTagIsUint8");

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