#include "sysmesbox.h"

#include "../../impacto.h"
#include "../../renderer2d.h"
#include "../../game.h"
#include "../../mem.h"
#include "../../scriptvars.h"
#include "../../profile/hud/sysmesbox.h"
#include "../../profile/games/mo6tw/sysmesbox.h"
#include "../../profile/dialogue.h"

namespace Impacto {
namespace MO6TW {

using namespace Impacto::Profile::SysMesBox;
using namespace Impacto::Profile::MO6TW::SysMesBox;

static float BoxAnimCount = 0.0f;
static float BoxTopY = 0.0f;
static float BoxBottomY = 0.0f;
static float LineLength = 0.0f;
static float BoxHeight = 0.0f;
static float BoxProgressCount = 0.0f;
static int TextStartCount = 0;

void SysMesBox::Show() { State = Showing; }
void SysMesBox::Hide() { State = Hiding; }

void SysMesBox::Update(float dt) {
  FadeAnimation.Update(dt);

  if (State == Hiding) {
    BoxAnimCount -= AnimationSpeed * dt;
    if (BoxAnimCount <= 0.0f) {
      BoxAnimCount = 0.0f;
      State = Hidden;
    }
  } else if (State == Showing) {
    BoxAnimCount += AnimationSpeed * dt;
    if (BoxAnimCount >= ScrWork[SW_SYSMESANIMCTF]) {
      BoxAnimCount = ScrWork[SW_SYSMESANIMCTF];
      State = Shown;
    }
  }

  ScrWork[SW_SYSMESANIMCTCUR] = std::floor(BoxAnimCount);

  if (State != Hidden) {
    if (State == Showing && FadeAnimation.IsOut())
      FadeAnimation.StartIn();
    else if (State == Hiding && FadeAnimation.IsIn())
      FadeAnimation.StartOut();
  }
}

void SysMesBox::Render() {
  if (BoxOpacity) {
    glm::vec4 col(1.0f, 1.0f, 1.0f, FadeAnimation.Progress);

    float diff = 0.0f;
    float maxWidth = FLT_MIN;
    for (int i = 0; i < MessageCount; i++) {
      if (maxWidth < MessageWidths[i]) maxWidth = MessageWidths[i];
    }
    if (maxWidth < BoxMinimumWidth) maxWidth = BoxMinimumWidth;

    Renderer2D::DrawSprite(BoxPartLeft,
                           glm::vec2(BoxX - (maxWidth / 2.0f), BoxY), col);

    float remainWidth = maxWidth - BoxMiddleRemainBase;
    float currentX = BoxMiddleBaseX - (maxWidth / 2.0f);
    while (remainWidth >= BoxMiddleBaseWidth) {
      Renderer2D::DrawSprite(BoxPartMiddle, glm::vec2(currentX, BoxY), col);
      currentX += BoxMiddleBaseWidth;
      remainWidth -= BoxMiddleBaseWidth;
    }

    BoxPartRight.Bounds.X = BoxRightBaseX - (remainWidth + BoxRightRemainPad);
    BoxPartRight.Bounds.Width = (remainWidth + BoxRightRemainPad) - 1.0f;
    Renderer2D::DrawSprite(BoxPartRight, glm::vec2(currentX, BoxY), col);

    // TODO: Draw Yes/No/OK buttons here

    for (int i = 0; i < MessageCount; i++) {
      diff = Messages[i][0].DestRect.X - (TextX - (maxWidth / 2.0f));
      for (int j = 0; j < MessageLengths[i]; j++) {
        Messages[i][j].Colors = Profile::Dialogue::ColorTable[0];
        Messages[i][j].DestRect.X -= diff;
        Messages[i][j].DestRect.Y = TextMiddleY + (i * TextLineHeight);
      }
      Renderer2D::DrawProcessedText(Messages[i], MessageLengths[i],
                                    Profile::Dialogue::DialogueFont, col.a);
    }
  }
}

}  // namespace MO6TW
}  // namespace Impacto