#include "sysmesbox.h"

#include "../../impacto.h"
#include "../../renderer2d.h"
#include "../../game.h"
#include "../../mem.h"
#include "../../profile/scriptvars.h"
#include "../../profile/hud/sysmesbox.h"
#include "../../profile/games/chlcc/sysmesbox.h"
#include "../../profile/dialogue.h"
#include "../../vm/interface/input.h"

namespace Impacto {
namespace CHLCC {

using namespace Impacto::Profile::SysMesBox;
using namespace Impacto::Profile::CHLCC::SysMesBox;
using namespace Impacto::Profile::ScriptVars;

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

  if (Vm::Interface::PAD1A & Vm::Interface::PADinputWentDown) {
    ChoiceMade = true;
  }

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
    Renderer2D::DrawSprite(Box, glm::vec2(BoxX, BoxY), col);

    // TODO: Draw Yes/No/OK buttons here

    float diff = 0.0f;
    float maxWidth = FLT_MIN;
    for (int i = 0; i < MessageCount; i++) {
      if (maxWidth < MessageWidths[i]) maxWidth = MessageWidths[i];
    }

    for (int i = 0; i < MessageCount; i++) {
      diff = Messages[i][0].DestRect.X - (TextX - (maxWidth / 2.0f));
      for (int j = 0; j < MessageLengths[i]; j++) {
        Messages[i][j].Colors = Profile::Dialogue::ColorTable[0];
        Messages[i][j].DestRect.X -= diff;
        Messages[i][j].DestRect.Y = TextMiddleY + (i * TextLineHeight);
      }
      Renderer2D::DrawProcessedText(Messages[i], MessageLengths[i],
                                    Profile::Dialogue::DialogueFont, col.a,
                                    true);
    }
  }
}

}  // namespace CHLCC
}  // namespace Impacto