#include "sysmesbox.h"

#include "../../profile/ui/sysmesbox.h"
#include "../../profile/games/rne/sysmesbox.h"
#include "../../profile/dialogue.h"
#include "../../profile/vm.h"
#include "../../profile/scriptvars.h"
#include "../../mem.h"
#include "../../inputsystem.h"
#include "../../renderer/renderer.h"

namespace Impacto {
namespace UI {
namespace RNE {

using namespace Impacto::UI::Widgets;
using namespace Impacto::Profile::ScriptVars;
using namespace Impacto::Profile::SysMesBox;
using namespace Impacto::Profile::RNE::SysMesBox;

static float BoxAnimCount = 0.0f;
static float BoxTopY = 0.0f;
static float BoxBottomY = 0.0f;
static float LineLength = 0.0f;
static float BoxHeight = 0.0f;
static float BoxProgressCount = 0.0f;
static float ButtonYesX = 0.0f;
static float ButtonRightX = 0.0f;
static int TextStartCount = 0;

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

  float textBeginY = TextMiddleY - (TextMarginY * (4 + MessageCount));
  for (int i = 0; i < MessageCount; i++) {
    for (ProcessedTextGlyph& glyph : Messages[i]) {
      if (glyph.CharId == 0) break;
      glyph.DestRect.Y = textBeginY + (i * TextLineHeight);
    }

    Label* message = new Label(Messages[i], MessageWidths[i], TextFontSize,
                               RendererOutlineMode::None);

    MessageItems->Add(message, FDIR_DOWN);
  }

  if (ChoiceCount == 1) {
    WidgetOK = new Button(0, ButtonOK, ButtonOKHighlighted, nullSprite,
                          glm::vec2(ButtonRightX, 0.0f));
    WidgetOK->OnClickHandler = onClick;
    ChoiceItems->Add(WidgetOK, FDIR_RIGHT);

  } else if (ChoiceCount == 2) {
    WidgetYes = new Button(0, ButtonYes, ButtonYesHighlighted, nullSprite,
                           glm::vec2(ButtonYesX, 0.0f));
    WidgetYes->OnClickHandler = onClick;
    ChoiceItems->Add(WidgetYes, FDIR_RIGHT);
    WidgetNo = new Button(1, ButtonNo, ButtonNoHighlighted, nullSprite,
                          glm::vec2(ButtonRightX, 0.0f));
    WidgetNo->OnClickHandler = onClick;
    ChoiceItems->Add(WidgetNo, FDIR_LEFT);
  }

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

  if (Profile::Vm::GameInstructionSet == +Vm::InstructionSet::RNE) {
    if (!ScrWork[SW_SYSMESANIMCTCUR]) {
      Audio::Channels[Audio::AC_SSE]->Play("sysse", 16, false, 0.0f);
    }
  }
}

void SysMesBox::Hide() {
  if (Profile::Vm::GameInstructionSet == +Vm::InstructionSet::RNE) {
    Audio::Channels[Audio::AC_SSE]->Play("sysse", 29, false, 0.0f);
  }
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

  if (State == Hiding) {
    BoxAnimCount -= AnimationSpeed * dt;
    if (BoxAnimCount <= 0.0f) {
      BoxAnimCount = 0.0f;
      State = Hidden;
    }
  } else if (State == Showing) {
    BoxAnimCount += AnimationSpeed * dt;
    if (BoxAnimCount >= ScrWork[SW_SYSMESANIMCTF]) {
      BoxAnimCount = (float)ScrWork[SW_SYSMESANIMCTF];
      State = Shown;
    }
  }

  ScrWork[SW_SYSMESANIMCTCUR] = (int)std::floor(BoxAnimCount);

  if (State != Hidden) {
    float linePosX = LinePositionXFirst;
    LineLength = LineWidthFirst;
    if (ScrWork[SW_SYSMESANIMCTCUR] > 1) {
      LineLength = LineWidthMultiplier * BoxAnimCount + LineWidthBase;
      linePosX = LinePositionX - LinePositionMultiplier * BoxAnimCount;
    }
    Line1.Bounds = RectF(linePosX + 1.0f, Line1SpriteY, LineLength + 2.0f,
                         LineSpriteHeight);
    Line2.Bounds = RectF(linePosX + 1.0f, Line2SpriteY, LineLength + 2.0f,
                         LineSpriteHeight);

    BoxProgressCount = BoxAnimCount - BoxDisplayStartCount;
    TextStartCount = 2 * MessageCount + 8;
    if (BoxProgressCount >= TextStartCount) {
      BoxHeight = BoxTextFontSize * MessageCount + BoxHeightBase;
    } else {
      BoxHeight = BoxHeightMultiplier * BoxProgressCount;
    }

    BoxTopY = BoxTopYBase - (BoxHeight / 2.0f);
    BoxBottomY = BoxTopY + BoxHeight;

    float labelButtonSpriteOffsetX =
        (MessageLabelSpriteMultiplier *
         (BoxProgressCount - 2 * MessageCount - 8));

    MessageLabel.Bounds =
        RectF(MessageLabelSpriteXBase - labelButtonSpriteOffsetX + 1.0f,
              MessageLabelSpriteY, labelButtonSpriteOffsetX - 2.0f,
              MessageLabelSpriteHeight);

    if (BoxProgressCount > TextStartCount) {
      if (State == Showing && FadeAnimation.IsOut())
        FadeAnimation.StartIn();
      else if (State == Hiding && FadeAnimation.IsIn())
        FadeAnimation.StartOut();
    }

    if (ChoiceCount == 2) {
      float buttonYesSpriteWidth = ButtonYWidthBase + labelButtonSpriteOffsetX;
      if (buttonYesSpriteWidth > ButtonWidth)
        buttonYesSpriteWidth = ButtonWidth;
      WidgetYes->NormalSprite.Bounds = RectF(
          WidgetYes->NormalSprite.Bounds.X, WidgetYes->NormalSprite.Bounds.Y,
          buttonYesSpriteWidth, WidgetYes->NormalSprite.Bounds.Height);
      WidgetYes->FocusedSprite.Bounds = RectF(
          WidgetYes->FocusedSprite.Bounds.X, WidgetYes->FocusedSprite.Bounds.Y,
          buttonYesSpriteWidth, WidgetYes->FocusedSprite.Bounds.Height);
      ButtonYesX = ButtonYesDisplayXBase - labelButtonSpriteOffsetX;
      WidgetYes->MoveTo(glm::vec2(ButtonYesX, BoxBottomY - ButtonYOffset));
    }
    if (ChoiceCount >= 1) {
      float buttonRightSpriteWidth =
          ButtonRightWidthBase + labelButtonSpriteOffsetX;
      ButtonRightX = ButtonRightDisplayXBase - buttonRightSpriteWidth;
      if (buttonRightSpriteWidth > ButtonWidth)
        buttonRightSpriteWidth = ButtonWidth;
      if (WidgetNo) {
        WidgetNo->NormalSprite.Bounds = RectF(
            WidgetNo->NormalSprite.Bounds.X, WidgetNo->NormalSprite.Bounds.Y,
            buttonRightSpriteWidth, WidgetNo->NormalSprite.Bounds.Height);
        WidgetNo->FocusedSprite.Bounds = RectF(
            WidgetNo->FocusedSprite.Bounds.X, WidgetNo->FocusedSprite.Bounds.Y,
            buttonRightSpriteWidth, WidgetNo->FocusedSprite.Bounds.Height);
        WidgetNo->MoveTo(glm::vec2(ButtonRightX, BoxBottomY - ButtonYOffset));
      }
      if (WidgetOK) {
        WidgetOK->NormalSprite.Bounds = RectF(
            WidgetOK->NormalSprite.Bounds.X, WidgetOK->NormalSprite.Bounds.Y,
            buttonRightSpriteWidth, WidgetOK->NormalSprite.Bounds.Height);
        WidgetOK->FocusedSprite.Bounds = RectF(
            WidgetOK->FocusedSprite.Bounds.X, WidgetOK->FocusedSprite.Bounds.Y,
            buttonRightSpriteWidth, WidgetOK->FocusedSprite.Bounds.Height);
        WidgetOK->MoveTo(glm::vec2(ButtonRightX, BoxBottomY - ButtonYOffset));
      }
    }

    if (Input::KeyboardButtonWentDown[SDL_SCANCODE_RIGHT] ||
        Input::KeyboardButtonWentDown[SDL_SCANCODE_LEFT]) {
      Audio::Channels[Audio::AC_SSE]->Play("sysse", 1, false, 0.0f);
    }

    if (IsFocused) {
      MessageItems->Update(dt);
      ChoiceItems->Update(dt);
    }
  }
}

void SysMesBox::Render() {
  if (State == Hidden) return;

  glm::vec4 col(1.0f, 1.0f, 1.0f, 1.0f);

  if (BoxAnimCount > BoxDisplayStartCount) {
    Renderer->DrawQuad(
        RectF(BoxDisplayX, BoxTopY - 1.0f, BoxWidth, BoxHeight + 2.0f),
        glm::vec4(1.0f, 1.0f, 1.0f, 0.75f));
    Renderer->DrawSprite(BoxDecorationTop,
                         glm::vec2(BoxDisplayX, BoxTopY - 3.0f), col);
    if (Type == +SysMesBoxType::Dash) {
      Renderer->DrawSprite(
          BoxDecorationBottom,
          glm::vec2(BoxDisplayX,
                    BoxBottomY - BoxDecorationBottom.Bounds.Height + 3.0f),
          col);
    } else {
      Renderer->DrawSprite(BoxDecorationBottom,
                           glm::vec2(BoxDisplayX, BoxBottomY - 3.0f), col);
    }

    if (BoxProgressCount > TextStartCount) {
      glm::vec4 texCol(1.0f, 1.0f, 1.0f, FadeAnimation.Progress);
      if (BoxHeight > TextDecorationStart) {
        Renderer->DrawSprite(
            TextDecoration,
            glm::vec2(BoxDisplayX, BoxTopY + TextDecorationTopYOffset), texCol);
        Renderer->DrawSprite(
            TextDecoration,
            glm::vec2(BoxDisplayX, BoxBottomY - TextDecorationBottomYOffset),
            texCol);
      }
      Renderer->DrawSprite(MessageLabel, glm::vec2(BoxDisplayX, BoxTopY + 3.0f),
                           texCol);

      MessageItems->Tint.a = texCol.a;
      MessageItems->Render();
      ChoiceItems->Tint.a = texCol.a;
      ChoiceItems->Render();
    }

  } else {
    Renderer->DrawSprite(
        Line1, glm::vec2(LineDisplayXBase - (LineLength / 2.0f), Line1DisplayY),
        col);
    Renderer->DrawSprite(
        Line2, glm::vec2(LineDisplayXBase - (LineLength / 2.0f), Line2DisplayY),
        col);
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
  for (const ProcessedTextGlyph& glyph : Messages[MessageCount]) {
    mesLen += glyph.DestRect.Width;
  }
  MessageWidths[MessageCount] = mesLen;
  MessageCount++;
}

void SysMesBox::AddChoice(Vm::BufferOffsetContext ctx) {
  // No text based choices in R;NE
  ChoiceCount++;
}

}  // namespace RNE
}  // namespace UI
}  // namespace Impacto