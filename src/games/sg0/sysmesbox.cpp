#include "sysmesbox.h"

#include "../../profile/games/sg0/sysmesbox.h"
#include "../../profile/ui/sysmesbox.h"
#include "../../profile/dialogue.h"
#include "../../profile/vm.h"
#include "../../profile/scriptvars.h"
#include "../../renderer/renderer.h"
#include "../../ui/ui.h"
#include "../../mem.h"
#include "../../inputsystem.h"
#include "../../vm/interface/input.h"
#include "../../audio/audiosystem.h"

namespace Impacto {
namespace UI {
namespace SG0 {

using namespace Impacto::Profile::SysMesBox;
using namespace Impacto::Profile::SG0::SysMesBox;
using namespace Impacto::Profile::ScriptVars;
using namespace Impacto::Vm::Interface;

void SysMesBox::Init() {
  ChoiceMade = false;
  MessageCount = 0;
  ChoiceCount = 0;
  Cursor = 0;

  FadeAnimation.DurationIn = FadeInDuration;
  FadeAnimation.DurationOut = FadeOutDuration;
  FadeAnimation.Direction = AnimationDirection::In;
  FadeAnimation.LoopMode = AnimationLoopMode::Stop;
}

void SysMesBox::AddMessage(Vm::BufferOffsetContext ctx) {
  if (MessageCount >= static_cast<int>(Messages.size())) return;
  Vm::Sc3VmThread dummy;
  dummy.IpOffset = ctx.IpOffset;
  dummy.ScriptBufferId = ctx.BufferId;
  dummy.UseMSBBuffers = Profile::Vm::UseMsbStrings;
  Messages[MessageCount] = TextLayoutPlainLine(
      &dummy, 255, *Profile::Dialogue::DialogueFont, TextFontSize,
      {MessageColor, OutlineColor}, 1.0f, glm::vec2(0.0f), TextAlignment::Left);
  MessageWidths[MessageCount] = GetTextWidth(Messages[MessageCount]);
  MessageCount++;
}

void SysMesBox::AddChoice(Vm::BufferOffsetContext ctx) {
  if (ChoiceCount >= static_cast<int>(Choices.size())) return;
  Vm::Sc3VmThread dummy;
  dummy.IpOffset = ctx.IpOffset;
  dummy.ScriptBufferId = ctx.BufferId;
  dummy.UseMSBBuffers = Profile::Vm::UseMsbStrings;
  Choices[ChoiceCount] = TextLayoutPlainLine(
      &dummy, 255, *Profile::Dialogue::DialogueFont, TextFontSize,
      {ChoiceColor, OutlineColor}, 1.0f, glm::vec2(0.0f), TextAlignment::Left);
  if (Choices[ChoiceCount].empty()) return;
  ChoiceWidths[ChoiceCount] = GetTextWidth(Choices[ChoiceCount]);
  ChoiceCount++;
}

void SysMesBox::Show() {
  const float blockHeight = MessageCount * TextLineHeight;
  const float firstLineY = TextMiddleY - blockHeight / 2.0f;
  for (int i = 0; i < MessageCount; i++) {
    if (Messages[i].empty()) continue;
    const glm::vec2 target(TextX - MessageWidths[i] / 2.0f,
                           firstLineY + i * TextLineHeight);
    const glm::vec2 offset = target - Messages[i][0].Position;
    for (ProcessedTextGlyph& glyph : Messages[i]) glyph.Move(offset);
  }

  float rowWidth = ChoiceSpacing * std::max(ChoiceCount - 1, 0);
  for (int i = 0; i < ChoiceCount; i++) rowWidth += ChoiceWidths[i];
  float x = TextX - rowWidth / 2.0f;
  const float choiceY = firstLineY + blockHeight + TextMarginY;
  for (int i = 0; i < ChoiceCount; i++) {
    const glm::vec2 offset = glm::vec2(x, choiceY) - Choices[i][0].Position;
    for (ProcessedTextGlyph& glyph : Choices[i]) glyph.Move(offset);
    ChoiceBounds[i] = RectF(x - ChoicePadding.x, choiceY - ChoicePadding.y,
                            ChoiceWidths[i] + 2 * ChoicePadding.x,
                            TextFontSize + 2 * ChoicePadding.y);
    x += ChoiceWidths[i] + ChoiceSpacing;
  }
  SetCursor(0);

  FadeAnimation.StartIn();
  State = Showing;
  if (UI::FocusedMenu != nullptr) {
    LastFocusedMenu = UI::FocusedMenu;
    LastFocusedMenu->IsFocused = false;
  }
  IsFocused = true;
  UI::FocusedMenu = this;
}

void SysMesBox::Hide() {
  FadeAnimation.StartOut();
  State = Hiding;
  if (LastFocusedMenu != nullptr) {
    UI::FocusedMenu = LastFocusedMenu;
    LastFocusedMenu->IsFocused = true;
  } else {
    UI::FocusedMenu = nullptr;
  }
  IsFocused = false;
}

void SysMesBox::SetCursor(int choice) {
  Cursor = choice;
  for (int i = 0; i < ChoiceCount; i++) {
    const uint32_t color = i == Cursor ? ChoiceHighlightColor : ChoiceColor;
    for (ProcessedTextGlyph& glyph : Choices[i]) glyph.Colors.TextColor = color;
  }
}

void SysMesBox::Choose(int choice) {
  ScrWork[SW_SYSSEL] = choice;
  ChoiceMade = true;
}

void SysMesBox::UpdateInput(float dt) {
  if (ChoiceCount == 0 || ChoiceMade) return;

  int next = Cursor;
  if (PADinputButtonWentDown & (PAD1LEFT | PAD1UP)) {
    next = (Cursor + ChoiceCount - 1) % ChoiceCount;
  } else if (PADinputButtonWentDown & (PAD1RIGHT | PAD1DOWN)) {
    next = (Cursor + 1) % ChoiceCount;
  }
  for (int i = 0; i < ChoiceCount; i++) {
    if (!ChoiceBounds[i].ContainsPoint(Input::CurMousePos)) continue;
    if (Input::PrevMousePos != Input::CurMousePos) next = i;
    if (Input::MouseButtonWentDown[SDL_BUTTON_LEFT]) {
      SetCursor(i);
      Choose(i);
      return;
    }
  }
  if (next != Cursor) {
    SetCursor(next);
    Audio::PlayInGroup(Audio::ACG_SE, "sysse", 1, false, 0);
  }

  if (PADinputButtonWentDown & PAD1A) {
    Choose(Cursor);
  } else if (PADinputButtonWentDown & PAD1B) {
    // Cancel picks the last choice ("No").
    SetCursor(ChoiceCount - 1);
    Choose(ChoiceCount - 1);
  }
}

void SysMesBox::Update(float dt) {
  FadeAnimation.Update(dt);
  if (State == Showing && FadeAnimation.IsIn()) State = Shown;
  if (State == Hiding && FadeAnimation.IsOut()) State = Hidden;
  if (State == Shown && IsFocused) UpdateInput(dt);
}

void SysMesBox::Render() {
  if (State == Hidden) return;
  const float alpha = FadeAnimation.Progress;

  Renderer->DrawSprite(BackgroundSprite, glm::vec2(0.0f),
                       glm::vec4(1.0f, 1.0f, 1.0f, alpha * BackgroundOpacity));
  for (int i = 0; i < MessageCount; i++) {
    Profile::Dialogue::DialogueFont->DrawProcessedText(
        Messages[i], alpha, RendererOutlineMode::Full);
  }
  for (int i = 0; i < ChoiceCount; i++) {
    Profile::Dialogue::DialogueFont->DrawProcessedText(
        Choices[i], alpha, RendererOutlineMode::Full);
  }
}

}  // namespace SG0
}  // namespace UI
}  // namespace Impacto
