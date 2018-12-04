#include "sysmesboxdisplay.h"
#include "../profile_internal.h"

namespace Impacto {
namespace Profile {
namespace SysMesBoxDisplay {

Sprite BoxDecorationTop;
Sprite BoxDecorationBottom;
Sprite TextDecoration;
Sprite MessageLabel;
Sprite Line1;
Sprite Line2;

float LinePositionXFirst;
float LinePositionX;
float LinePositionMultiplier;
float LineWidthFirst;
float LineWidthBase;
float LineWidthMultiplier;
float Line1SpriteY;
float Line2SpriteY;
float LineSpriteHeight;
float LineDisplayXBase;
float Line1DisplayY;
float Line2DisplayY;
float BoxDisplayStartCount;
float BoxHeightBase;
float BoxHeightMultiplier;
float BoxWidth;
float BoxTextFontSize;
float BoxTopYBase;
float BoxDisplayX;
float MessageLabelSpriteXBase;
float MessageLabelSpriteY;
float MessageLabelSpriteHeight;
float AnimationSpeed;
float FadeInDuration;
float FadeOutDuration;

void Configure() {
  auto const& _sysMesBoxDisplay =
      EnsureGetMemberOfType(Json, "/", "SysMesBoxDisplay", kObjectType);

  BoxDecorationTop = EnsureGetMemberSprite(
      _sysMesBoxDisplay, "/SysMesBoxDisplay", "BoxDecorationTop");
  BoxDecorationBottom = EnsureGetMemberSprite(
      _sysMesBoxDisplay, "/SysMesBoxDisplay", "BoxDecorationBottom");
  TextDecoration = EnsureGetMemberSprite(_sysMesBoxDisplay, "/SysMesBoxDisplay",
                                         "TextDecoration");
  MessageLabel = EnsureGetMemberSprite(_sysMesBoxDisplay, "/SysMesBoxDisplay",
                                       "MessageLabel");
  Line1 =
      EnsureGetMemberSprite(_sysMesBoxDisplay, "/SysMesBoxDisplay", "Line1");
  Line2 =
      EnsureGetMemberSprite(_sysMesBoxDisplay, "/SysMesBoxDisplay", "Line2");

  LinePositionXFirst = EnsureGetMemberFloat(
      _sysMesBoxDisplay, "/SysMesBoxDisplay", "LinePositionXFirst");
  LinePositionX = EnsureGetMemberFloat(_sysMesBoxDisplay, "/SysMesBoxDisplay",
                                       "LinePositionX");
  LinePositionMultiplier = EnsureGetMemberFloat(
      _sysMesBoxDisplay, "/SysMesBoxDisplay", "LinePositionMultiplier");
  LineWidthFirst = EnsureGetMemberFloat(_sysMesBoxDisplay, "/SysMesBoxDisplay",
                                        "LineWidthFirst");
  LineWidthBase = EnsureGetMemberFloat(_sysMesBoxDisplay, "/SysMesBoxDisplay",
                                       "LineWidthBase");
  LineWidthMultiplier = EnsureGetMemberFloat(
      _sysMesBoxDisplay, "/SysMesBoxDisplay", "LineWidthMultiplier");
  Line1SpriteY = EnsureGetMemberFloat(_sysMesBoxDisplay, "/SysMesBoxDisplay",
                                      "Line1SpriteY");
  Line2SpriteY = EnsureGetMemberFloat(_sysMesBoxDisplay, "/SysMesBoxDisplay",
                                      "Line2SpriteY");
  LineSpriteHeight = EnsureGetMemberFloat(
      _sysMesBoxDisplay, "/SysMesBoxDisplay", "LineSpriteHeight");
  LineDisplayXBase = EnsureGetMemberFloat(
      _sysMesBoxDisplay, "/SysMesBoxDisplay", "LineDisplayXBase");
  Line1DisplayY = EnsureGetMemberFloat(_sysMesBoxDisplay, "/SysMesBoxDisplay",
                                       "Line1DisplayY");
  Line2DisplayY = EnsureGetMemberFloat(_sysMesBoxDisplay, "/SysMesBoxDisplay",
                                       "Line2DisplayY");
  BoxDisplayStartCount = EnsureGetMemberFloat(
      _sysMesBoxDisplay, "/SysMesBoxDisplay", "BoxDisplayStartCount");
  BoxHeightBase = EnsureGetMemberFloat(_sysMesBoxDisplay, "/SysMesBoxDisplay",
                                       "BoxHeightBase");
  BoxHeightMultiplier = EnsureGetMemberFloat(
      _sysMesBoxDisplay, "/SysMesBoxDisplay", "BoxHeightMultiplier");
  BoxWidth =
      EnsureGetMemberFloat(_sysMesBoxDisplay, "/SysMesBoxDisplay", "BoxWidth");
  BoxTextFontSize = EnsureGetMemberFloat(_sysMesBoxDisplay, "/SysMesBoxDisplay",
                                         "BoxTextFontSize");
  BoxTopYBase = EnsureGetMemberFloat(_sysMesBoxDisplay, "/SysMesBoxDisplay",
                                     "BoxTopYBase");
  BoxDisplayX = EnsureGetMemberFloat(_sysMesBoxDisplay, "/SysMesBoxDisplay",
                                     "BoxDisplayX");
  MessageLabelSpriteXBase = EnsureGetMemberFloat(
      _sysMesBoxDisplay, "/SysMesBoxDisplay", "MessageLabelSpriteXBase");
  MessageLabelSpriteY = EnsureGetMemberFloat(
      _sysMesBoxDisplay, "/SysMesBoxDisplay", "MessageLabelSpriteY");
  MessageLabelSpriteHeight = EnsureGetMemberFloat(
      _sysMesBoxDisplay, "/SysMesBoxDisplay", "MessageLabelSpriteHeight");
  AnimationSpeed = EnsureGetMemberFloat(_sysMesBoxDisplay, "/SysMesBoxDisplay",
                                        "AnimationSpeed");
  FadeInDuration = EnsureGetMemberFloat(_sysMesBoxDisplay, "/SysMesBoxDisplay",
                                        "FadeInDuration");
  FadeOutDuration = EnsureGetMemberFloat(_sysMesBoxDisplay, "/SysMesBoxDisplay",
                                         "FadeOutDuration");
}

}  // namespace SysMesBoxDisplay
}  // namespace Profile
}  // namespace Impacto