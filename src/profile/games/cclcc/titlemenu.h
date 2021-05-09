#pragma once

#include "../../../spritesheet.h"
#include "../../../games/cclcc/titlemenu.h"

namespace Impacto {
namespace Profile {
namespace CCLCC {
namespace TitleMenu {

extern Sprite BackgroundSprite;
extern Sprite CopyrightTextSprite;
extern Sprite MainBackgroundSprite;
extern Sprite MenuSprite;
extern Sprite ItemHighlightSprite;

extern float CopyrightTextX;
extern float CopyrightTextY;
extern float MenuX;
extern float MenuY;
extern float ItemHighlightOffsetX;
extern float ItemHighlightOffsetY;
extern float ItemYBase;
extern float ItemPadding;

void Configure();



}  // namespace TitleMenu
}  // namespace CCLCC
}  // namespace Profile
}  // namespace Impacto