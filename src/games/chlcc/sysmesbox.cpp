#include "sysmesbox.h"

#include "../../profile/ui/sysmesbox.h"
#include "../../profile/games/chlcc/sysmesbox.h"
#include "../../profile/dialogue.h"
#include "../../profile/scriptvars.h"
#include "../../mem.h"
#include "../../renderer/renderer.h"
#include "../../util.h"

namespace Impacto {
namespace UI {
namespace CHLCC {

using namespace Impacto::UI::Widgets;
using namespace Impacto::Profile::ScriptVars;
using namespace Impacto::Profile::SysMesBox;
using namespace Impacto::Profile::CHLCC::SysMesBox;

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
  float maxWidth = FLT_MIN;
  for (int i = 0; i < MessageCount; i++) {
    if (maxWidth < MessageWidths[i]) maxWidth = MessageWidths[i];
  }
  if (maxWidth < MinHighlightWidth) maxWidth = MinHighlightWidth;

  for (int i = 0; i < MessageCount; i++) {
    diff = Messages[i][0].DestRect.X - (TextX - (maxWidth / 2.0f));
    for (ProcessedTextGlyph& glyph : Messages[i]) {
      glyph.Colors = Profile::Dialogue::ColorTable[0];
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
  if (maxWidth < totalChoiceWidth) maxWidth = totalChoiceWidth;
  if (maxWidth < MinMaxMesWidth) maxWidth = MinMaxMesWidth;

  ChoiceX = (maxWidth / 2.0f) - totalChoiceWidth + ChoiceXBase;

  float tempChoiceX = ChoiceX;

  for (int i = 0; i < ChoiceCount; i++) {
    diff = Choices[i][0].DestRect.X - tempChoiceX;
    for (ProcessedTextGlyph& choice : Choices[i]) {
      choice.Colors = Profile::Dialogue::ColorTable[0];
      choice.DestRect.X -= diff;
      choice.DestRect.Y = ChoiceY;
    }

    Button* choice = new Button(
        i, nullSprite, nullSprite, SelectionLeftPart,
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
  if (LastFocusedMenu != 0) {
    UI::FocusedMenu = LastFocusedMenu;
    LastFocusedMenu->IsFocused = true;
  } else {
    UI::FocusedMenu = 0;
  }
  IsFocused = false;
}

void SysMesBox::Update(float dt) {
  UpdateInput(dt);

  FadeAnimation.Update(dt);
  if (State != Hidden) {
    if (FadeAnimation.IsIn()) State = Shown;
    if (FadeAnimation.IsOut()) State = Hidden;

    if (IsFocused) {
      MessageItems->Update(dt);
      ChoiceItems->Update(dt);
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
  if (State != Hidden) {
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
        glm::vec2 position = LoadingStarsPosition +
                             glm::vec2(LoadingStar.ScaledWidth() * i, 0.0f);

        CornersQuad dest = LoadingStar.ScaledBounds()
                               .RotateAroundCenter(star.Angle)
                               .Translate(position);

        float alpha = LoadingStarsFadeAnimation.Progress;
        Renderer->DrawSprite(LoadingStar, dest, {1.0f, 1.0f, 1.0f, alpha});
      }
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
                          TextFontSize, Profile::Dialogue::ColorTable[10], 1.0f,
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
                          TextFontSize, Profile::Dialogue::ColorTable[10], 1.0f,
                          glm::vec2(TextX, 0.0f), TextAlignment::Left);
  float mesLen = 0.0f;
  for (size_t i = 0; i < Choices[ChoiceCount].size(); i++) {
    mesLen += Choices[ChoiceCount][i].DestRect.Width;
  }
  ChoiceWidths[ChoiceCount] = mesLen;
  ChoiceCount++;
}

}  // namespace CHLCC
}  // namespace UI
}  // namespace Impacto