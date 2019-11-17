#include "sysmesbox.h"

#include "../../impacto.h"
#include "../../renderer2d.h"
#include "../../game.h"
#include "../../mem.h"
#include "../../inputsystem.h"
#include "../../vm/interface/input.h"
#include "../../profile/scriptvars.h"
#include "../../profile/hud/sysmesbox.h"
#include "../../profile/games/rne/sysmesbox.h"
#include "../../profile/dialogue.h"
#include "../../profile/vm.h"

namespace Impacto {
namespace RNE {

using namespace Impacto::Profile::SysMesBox;
using namespace Impacto::Profile::RNE::SysMesBox;
using namespace Impacto::Profile::ScriptVars;

static float BoxAnimCount = 0.0f;
static float BoxTopY = 0.0f;
static float BoxBottomY = 0.0f;
static float LineLength = 0.0f;
static float BoxHeight = 0.0f;
static float BoxProgressCount = 0.0f;
static float ButtonYesX = 0.0f;
static float ButtonRightX = 0.0f;
static int TextStartCount = 0;

void SysMesBox::Show() {
  State = Showing;
  // Hack...
  if (Profile::Vm::GameInstructionSet == +Vm::InstructionSet::RNE) {
    if (!ScrWork[SW_SYSMESANIMCTCUR]) {
      Io::InputStream* stream;
      Io::VfsOpen("sysse", 16, &stream);
      Audio::Channels[Audio::AC_SSE].Play(Audio::AudioStream::Create(stream),
                                          false, 0.0f);
    }
  }
}
void SysMesBox::Hide() {
  State = Hiding;
  // Hack...
  if (Profile::Vm::GameInstructionSet == +Vm::InstructionSet::RNE) {
    if (ScrWork[SW_SYSMESANIMCTCUR] == ScrWork[SW_SYSMESANIMCTF]) {
      Io::InputStream* stream;
      Io::VfsOpen("sysse", 29, &stream);
      Audio::Channels[Audio::AC_SSE].Play(Audio::AudioStream::Create(stream),
                                          false, 0.0f);
    }
  }
}

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

    if (ChoiceCount == 2) {
      float buttonYesSpriteY = ButtonSpriteY;
      float buttonYesSpriteWidth = ButtonYWidthBase + labelButtonSpriteOffsetX;
      if (buttonYesSpriteWidth > ButtonWidth)
        buttonYesSpriteWidth = ButtonWidth;
      if (CurrentChoice == 0) buttonYesSpriteY = ButtonSelectedSpriteY;
      ButtonYes.Bounds = RectF(ButtonYes.Bounds.X, buttonYesSpriteY,
                               buttonYesSpriteWidth, ButtonYes.Bounds.Height);
      ButtonYesX = ButtonYesDisplayXBase - labelButtonSpriteOffsetX;
    }
    if (ChoiceCount >= 1) {
      float buttonRightSpriteY = ButtonSpriteY;
      float buttonRightSpriteWidth =
          ButtonRightWidthBase + labelButtonSpriteOffsetX;
      ButtonRightX = ButtonRightDisplayXBase - buttonRightSpriteWidth;
      if (buttonRightSpriteWidth > ButtonWidth)
        buttonRightSpriteWidth = ButtonWidth;
      if (CurrentChoice == 1 || (ChoiceCount == 1 && CurrentChoice == 0))
        buttonRightSpriteY = ButtonSelectedSpriteY;
      ButtonNo.Bounds = RectF(ButtonNo.Bounds.X, buttonRightSpriteY,
                              buttonRightSpriteWidth, ButtonNo.Bounds.Height);
      ButtonOK.Bounds = RectF(ButtonOK.Bounds.X, buttonRightSpriteY,
                              buttonRightSpriteWidth, ButtonOK.Bounds.Height);
    }

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

    if (Type == +SysMesBoxType::Dash) {
      float boxDecorationHeight = BoxHeight / 2.0f;
      if (boxDecorationHeight > 60.0f) boxDecorationHeight = 61.0f;

      BoxDecorationTop.Bounds =
          RectF(BoxDecorationTop.Bounds.X, BoxDecorationTop.Bounds.Y,
                BoxDecorationTop.Bounds.Width, boxDecorationHeight);
      BoxDecorationBottom.Bounds =
          RectF(BoxDecorationBottom.Bounds.X,
                Line1DisplayY - boxDecorationHeight + 1.0f,
                BoxDecorationBottom.Bounds.Width, boxDecorationHeight);
    }

    // Nice input
    if (Input::KeyboardButtonWentDown[SDL_SCANCODE_RIGHT]) {
      Io::InputStream* stream;
      Io::VfsOpen("sysse", 1, &stream);
      Audio::Channels[Audio::AC_SSE].Play(Audio::AudioStream::Create(stream),
                                          false, 0.0f);
      if (CurrentChoice == 255)
        CurrentChoice = 1;
      else {
        CurrentChoice++;
        if (CurrentChoice > 1) CurrentChoice = 0;
      }
    } else if (Input::KeyboardButtonWentDown[SDL_SCANCODE_LEFT]) {
      Io::InputStream* stream;
      Io::VfsOpen("sysse", 1, &stream);
      Audio::Channels[Audio::AC_SSE].Play(Audio::AudioStream::Create(stream),
                                          false, 0.0f);
      if (CurrentChoice == 255)
        CurrentChoice = 0;
      else {
        CurrentChoice--;
        if (CurrentChoice < 0) CurrentChoice = 1;
      }
    } else if (Vm::Interface::PAD1A & Vm::Interface::PADinputWentDown) {
      ChoiceMade = true;
    }
  }
}

void SysMesBox::Render() {
  if (BoxOpacity) {
    glm::vec4 col(1.0f, 1.0f, 1.0f, glm::smoothstep(0.0f, 1.0f, BoxOpacity));

    if (BoxAnimCount > BoxDisplayStartCount) {
      Renderer2D::DrawRect(
          RectF(BoxDisplayX, BoxTopY - 1.0f, BoxWidth, BoxHeight + 2.0f),
          glm::vec4(1.0f, 1.0f, 1.0f, 0.75f));
      Renderer2D::DrawSprite(BoxDecorationTop,
                             glm::vec2(BoxDisplayX, BoxTopY - 3.0f), col);
      if (Type == +SysMesBoxType::Dash) {
        Renderer2D::DrawSprite(
            BoxDecorationBottom,
            glm::vec2(BoxDisplayX,
                      BoxBottomY - BoxDecorationBottom.Bounds.Height + 3.0f),
            col);
      } else {
        Renderer2D::DrawSprite(BoxDecorationBottom,
                               glm::vec2(BoxDisplayX, BoxBottomY - 3.0f), col);
      }

      if (BoxProgressCount > TextStartCount) {
        glm::vec4 texCol(1.0f, 1.0f, 1.0f, FadeAnimation.Progress);
        if (BoxHeight > TextDecorationStart) {
          Renderer2D::DrawSprite(
              TextDecoration,
              glm::vec2(BoxDisplayX, BoxTopY + TextDecorationTopYOffset),
              texCol);
          Renderer2D::DrawSprite(
              TextDecoration,
              glm::vec2(BoxDisplayX, BoxBottomY - TextDecorationBottomYOffset),
              texCol);
        }
        Renderer2D::DrawSprite(MessageLabel,
                               glm::vec2(BoxDisplayX, BoxTopY + 3.0f), texCol);

        if (ChoiceCount == 2) {
          Renderer2D::DrawSprite(
              ButtonYes, glm::vec2(ButtonYesX, BoxBottomY - ButtonYOffset),
              texCol);
          Renderer2D::DrawSprite(
              ButtonNo, glm::vec2(ButtonRightX, BoxBottomY - ButtonYOffset),
              texCol);
        } else if (ChoiceCount == 1) {
          Renderer2D::DrawSprite(
              ButtonOK, glm::vec2(ButtonRightX, BoxBottomY - ButtonYOffset),
              texCol);
        }

        float textBeginY = TextMiddleY - (TextMarginY * (4 + MessageCount));
        for (int i = 0; i < MessageCount; i++) {
          int lineLen;
          for (lineLen = 0; lineLen < 255; lineLen++) {
            if (Messages[i][lineLen].CharId == 0) break;
            Messages[i][lineLen].DestRect.Y = textBeginY + (i * TextLineHeight);
          }
          Renderer2D::DrawProcessedText(
              Messages[i], lineLen, Profile::Dialogue::DialogueFont, texCol.a);
        }
      }

    } else {
      Renderer2D::DrawSprite(
          Line1,
          glm::vec2(LineDisplayXBase - (LineLength / 2.0f), Line1DisplayY),
          col);
      Renderer2D::DrawSprite(
          Line2,
          glm::vec2(LineDisplayXBase - (LineLength / 2.0f), Line2DisplayY),
          col);
    }
  }
}

}  // namespace RNE
}  // namespace Impacto