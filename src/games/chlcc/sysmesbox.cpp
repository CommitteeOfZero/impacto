#include "sysmesbox.h"

#include "../../profile/ui/sysmesbox.h"
#include "../../profile/games/chlcc/sysmesbox.h"
#include "../../profile/dialogue.h"
#include "../../profile/scriptvars.h"
#include "../../mem.h"
#include "../../renderer/renderer.h"
#include "../../util.h"
#include "../../ui/widgets/chlcc/systemmessagebutton.h"

namespace Impacto {
namespace UI {
namespace CHLCC {

using namespace Impacto::UI::Widgets;
using namespace Impacto::Profile::ScriptVars;
using namespace Impacto::Profile::SysMesBox;
using namespace Impacto::Profile::CHLCC::SysMesBox;
using namespace Impacto::UI::Widgets::CHLCC;

SysMesBox::SysMesBox() {
  LoadingStarsFadeAnimation.DurationIn = LoadingStarsFadeDuration;
}

void SysMesBox::ChoiceItemOnClick(Button* target) {
  ScrWork[SW_SYSSEL] = target->Id;
  ChoiceMade = true;
}

void SysMesBox::Show() {
  MessageItems = new Widgets::Group(this);
  ChoiceItems = new Widgets::Group(this);

  Sprite nullSprite = Sprite();
  nullSprite.Bounds = RectF(0.0f, 0.0f, 0.0f, 0.0f);

  auto onClick = [this](auto* btn) { return ChoiceItemOnClick(btn); };

  float diff = 0.0f;
  float maxWidth = MinHighlightWidth;
  for (int i = 0; i < MessageCount; i++) {
    maxWidth = std::max(maxWidth, MessageWidths[i]);
  }

  for (int i = 0; i < MessageCount; i++) {
    if (Messages[i].empty()) continue;

    diff = Messages[i][0].DestRect.X - (TextX - (maxWidth / 2.0f));
    for (ProcessedTextGlyph& glyph : Messages[i]) {
      glyph.DestRect.X -= diff;
      glyph.DestRect.Y = TextMiddleY + (i * TextLineHeight);
    }

    Label* message = new Label(Messages[i], MessageWidths[i], TextFontSize,
                               RendererOutlineMode::Full);

    MessageItems->Add(message, FDIR_DOWN);
  }

  float totalChoiceWidth = 0.0f;
  for (int i = 0; i < ChoiceCount; i++) {
    totalChoiceWidth += ChoiceWidths[i] + ChoicePadding;
  }
  maxWidth = std::max(maxWidth, totalChoiceWidth);
  maxWidth = std::max(maxWidth, MinMaxMesWidth);

  ChoiceX = (maxWidth / 2.0f) - totalChoiceWidth + ChoiceXBase;

  float tempChoiceX = ChoiceX;

  for (int i = 0; i < ChoiceCount; i++) {
    diff = Choices[i][0].DestRect.X - tempChoiceX;
    for (ProcessedTextGlyph& choice : Choices[i]) {
      choice.DestRect.X -= diff;
      choice.DestRect.Y = ChoiceY;
    }

    Button* choice = new SystemMessageButton(
        i, nullSprite, nullSprite, SelectionLeftPart, SelectionMiddlePart,
        SelectionRightPart,
        glm::vec2(Choices[i][0].DestRect.X, Choices[i][0].DestRect.Y));
    choice->HighlightOffset = glm::vec2(HighlightXOffset, HighlightYOffset);

    choice->SetText(Choices[i], ChoiceWidths[i],
                    Profile::Dialogue::DefaultFontSize,
                    RendererOutlineMode::Full);
    choice->OnClickHandler = onClick;

    ChoiceItems->Add(choice, FDIR_LEFT);

    tempChoiceX += ChoiceWidths[i] + ChoicePadding;
  }

  FadeAnimation.StartIn();
  MessageItems->Show();
  MessageItems->HasFocus = false;
  if (ChoiceCount != 0) ChoiceItems->Show();
  State = Showing;

  if (UI::FocusedMenu != 0) {
    LastFocusedMenu = UI::FocusedMenu;
    LastFocusedMenu->IsFocused = false;
  }
  IsFocused = true;
  UI::FocusedMenu = this;

  LoadingStarsFadeAnimation.StartIn(true);
  for (SysMesBoxStar& star : LoadingStars) {
    star.Angle = ScrWorkAngleToRad(CALCrnd(8192) << 3);
    star.RotationSpeed = ScrWorkAngleToRad(CALCrnd(4096) - 2048);
  }
}

void SysMesBox::Hide() {
  FadeAnimation.StartOut();
  State = Hiding;
}

void SysMesBox::Update(float dt) {
  UpdateInput(dt);

  FadeAnimation.Update(dt);
  if (State != Hidden) {
    if (State == Hiding && FadeAnimation.IsOut()) {
      if (LastFocusedMenu != 0) {
        UI::FocusedMenu = LastFocusedMenu;
        LastFocusedMenu->IsFocused = true;
      } else {
        UI::FocusedMenu = 0;
      }
      IsFocused = false;
    }

    if (FadeAnimation.IsIn()) State = Shown;
    if (FadeAnimation.IsOut()) State = Hidden;

    if (IsFocused) {
      MessageItems->Update(dt);
      MessageItems->UpdateInput(dt);
      ChoiceItems->Update(dt);
      ChoiceItems->UpdateInput(dt);
    }

    if (GetFlag(SF_SAVEICON)) {
      LoadingStarsFadeAnimation.Update(dt);
      for (SysMesBoxStar& star : LoadingStars) {
        star.Angle += star.RotationSpeed;
      }
    }
  }
}

void SysMesBox::Render() {
  if (State == Hidden) return;

  glm::vec4 col(1.0f, 1.0f, 1.0f, FadeAnimation.Progress);

  float maxWidth = FLT_MIN;
  for (int i = 0; i < MessageCount; i++) {
    if (maxWidth < MessageWidths[i]) maxWidth = MessageWidths[i];
  }
  if (maxWidth < MinMaxMesWidth) maxWidth = MinMaxMesWidth;

  Renderer->DrawSprite(Box, glm::vec2(BoxX, BoxY), col);

  MessageItems->Tint.a = FadeAnimation.Progress;
  MessageItems->Render();
  ChoiceItems->Tint.a = FadeAnimation.Progress;
  ChoiceItems->Render();

  if (GetFlag(SF_SAVEICON)) {
    for (size_t i = 0; i < LoadingStars.size(); i++) {
      const SysMesBoxStar& star = LoadingStars[i];
      glm::vec2 position =
          LoadingStarsPosition + glm::vec2(LoadingStar.ScaledWidth() * i, 0.0f);

      CornersQuad dest = LoadingStar.ScaledBounds()
                             .RotateAroundCenter(star.Angle)
                             .Translate(position);

      float alpha = LoadingStarsFadeAnimation.Progress;
      Renderer->DrawSprite(LoadingStar, dest, {1.0f, 1.0f, 1.0f, alpha});
    }
  }
}

void SysMesBox::Init() {
  ChoiceMade = false;
  MessageCount = 0;
  ChoiceCount = 0;

  FadeAnimation.DurationIn = FadeInDuration;
  FadeAnimation.DurationOut = FadeOutDuration;
  FadeAnimation.Direction = AnimationDirection::In;
  FadeAnimation.LoopMode = AnimationLoopMode::Stop;
}

void SysMesBox::AddMessage(Vm::BufferOffsetContext ctx) {
  Impacto::Vm::Sc3VmThread dummy;
  dummy.IpOffset = ctx.IpOffset;
  dummy.ScriptBufferId = ctx.ScriptBufferId;
  Messages[MessageCount] =
      TextLayoutPlainLine(&dummy, 255, Profile::Dialogue::DialogueFont,
                          TextFontSize, Profile::Dialogue::ColorTable[0], 1.0f,
                          glm::vec2(TextX, 0.0f), TextAlignment::Left);
  float mesLen = 0.0f;
  for (size_t i = 0; i < Messages[MessageCount].size(); i++) {
    mesLen += Messages[MessageCount][i].DestRect.Width;
  }
  MessageWidths[MessageCount] = mesLen;
  MessageCount++;
}

void SysMesBox::AddChoice(Vm::BufferOffsetContext ctx) {
  Impacto::Vm::Sc3VmThread dummy;
  dummy.IpOffset = ctx.IpOffset;
  dummy.ScriptBufferId = ctx.ScriptBufferId;
  Choices[ChoiceCount] =
      TextLayoutPlainLine(&dummy, 255, Profile::Dialogue::DialogueFont,
                          TextFontSize, Profile::Dialogue::ColorTable[0], 1.0f,
                          glm::vec2(TextX, 0.0f), TextAlignment::Left);
  float mesLen = 0.0f;
  if (Choices[ChoiceCount].size() != 0) {
    const RectF firstGlyph = Choices[ChoiceCount][0].DestRect;
    const size_t lastIndex = Choices[ChoiceCount].size() - 1;
    const RectF lastGlyph = Choices[ChoiceCount][lastIndex].DestRect;
    mesLen = lastGlyph.X + lastGlyph.Width - firstGlyph.X;
  }
  ChoiceWidths[ChoiceCount] = mesLen;
  ChoiceCount++;
}

}  // namespace CHLCC
}  // namespace UI
}  // namespace Impacto