#include "sysmesbox.h"

#include "../../profile/ui/sysmesbox.h"
#include "../../profile/games/cc/sysmesbox.h"
#include "../../profile/dialogue.h"
#include "../../profile/game.h"
#include "../../profile/scriptvars.h"
#include "../../mem.h"
#include "../../renderer/renderer.h"

namespace Impacto {
namespace UI {
namespace CC {

using namespace Impacto::UI::Widgets;
using namespace Impacto::Profile::ScriptVars;
using namespace Impacto::Profile::SysMesBox;
using namespace Impacto::Profile::CC::SysMesBox;

static float BoxAnimCount = 0.0f;

void SysMesBox::ChoiceItemOnClick(Button* target) {
  ScrWork[SW_SYSSEL] = target->Id;
  ChoiceMade = true;
}

void SysMesBox::Show() {
  MessageItems = new Widgets::Group(this);
  ChoiceItems = new Widgets::Group(this);

  Sprite nullSprite = Sprite();
  nullSprite.Bounds = RectF(0.0f, 0.0f, 0.0f, 0.0f);

  auto onClick =
      std::bind(&SysMesBox::ChoiceItemOnClick, this, std::placeholders::_1);

  float textBeginY = (TextMiddleY - TextMarginY * MessageCount) / 2.0f;
  float diff;
  float maxWidth = FLT_MIN;
  for (int i = 0; i < MessageCount; i++) {
    if (maxWidth < MessageWidths[i]) maxWidth = MessageWidths[i];
  }

  for (int i = 0; i < MessageCount; i++) {
    diff = Messages[i][0].DestRect.X - ((TextX - maxWidth) / 2.0f);
    for (int j = 0; j < MessageLengths[i]; j++) {
      if (Messages[i][j].CharId == 0) break;
      Messages[i][j].DestRect.X -= diff;
      Messages[i][j].DestRect.Y = textBeginY + (TextLineHeight * i);
    }

    Label* message = new Label(Messages[i], MessageLengths[i], MessageWidths[i],
                               TextFontSize, RO_Full);

    MessageItems->Add(message, FDIR_DOWN);
  }

  if (ChoiceCount == 1) {
    WidgetOK = new Button(
        0, ButtonOK, ButtonOKHighlighted, nullSprite,
        glm::vec2(ButtonOKCenterPosX - (ButtonOK.Bounds.Width / 2),
                  ButtonOKCenterPosY - (ButtonOK.Bounds.Height / 2)));
    WidgetOK->OnClickHandler = onClick;
    ChoiceItems->Add(WidgetOK, FDIR_RIGHT);

  } else if (ChoiceCount == 2) {
    WidgetYes = new Button(
        0, ButtonYes, ButtonYesHighlighted, nullSprite,
        glm::vec2(ButtonYesCenterPosX - (ButtonYes.Bounds.Width / 2),
                  ButtonYesCenterPosY - (ButtonYes.Bounds.Height / 2)));
    WidgetYes->OnClickHandler = onClick;
    ChoiceItems->Add(WidgetYes, FDIR_RIGHT);
    WidgetNo = new Button(
        1, ButtonNo, ButtonNoHighlighted, nullSprite,
        glm::vec2(ButtonNoCenterPosX - (ButtonNo.Bounds.Width / 2),
                  ButtonNoCenterPosY - (ButtonNo.Bounds.Height / 2)));
    WidgetNo->OnClickHandler = onClick;
    ChoiceItems->Add(WidgetNo, FDIR_LEFT);
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
  UpdateInput();

  FadeAnimation.Update(dt);

  if (State == Hiding) {
    BoxAnimCount = FadeAnimation.Progress * ScrWork[SW_SYSMESANIMCTF];
    if (BoxAnimCount <= 0.0f) {
      BoxAnimCount = 0.0f;
      State = Hidden;
    }
  } else if (State == Showing) {
    BoxAnimCount = FadeAnimation.Progress * ScrWork[SW_SYSMESANIMCTF];
    if (BoxAnimCount >= ScrWork[SW_SYSMESANIMCTF]) {
      BoxAnimCount = ScrWork[SW_SYSMESANIMCTF];
      State = Shown;
    }
  }

  ScrWork[SW_SYSMESANIMCTCUR] = std::floor(BoxAnimCount);

  float animationProgress = FadeAnimation.Progress * AnimationSpeed;
  int animationFrame = std::ceil(animationProgress);

  if (animationProgress > SealSpriteCount) {
    if (ChoiceCount == 2) {
      float offsetProgress =
          std::min(animationProgress - AnimationProgressWidgetsStartOffset,
                   ButtonYesAnimationProgressEnd);
      float scaleYes =
          ButtonScaleMax - (offsetProgress * ButtonYesNoScaleMultiplier);
      WidgetYes->NormalSprite.BaseScale = glm::vec2(scaleYes);
      WidgetYes->FocusedSprite.BaseScale = glm::vec2(scaleYes);
      WidgetYes->MoveTo(glm::vec2(
          ButtonYesCenterPosX - (WidgetYes->NormalSprite.ScaledWidth() * 0.5f),
          ButtonYesCenterPosY -
              (WidgetYes->NormalSprite.ScaledHeight() * 0.5f)));
      WidgetYes->Tint =
          glm::vec4(1.0f, 1.0f, 1.0f, offsetProgress / ButtonYesNoAlphaDivider);

      if (animationProgress - AnimationProgressWidgetsStartOffset >
          ButtonNoDisplayStart) {
        float offsetProgress =
            (animationProgress - ButtonNoAnimationProgressOffset);
        float scaleNo =
            ButtonScaleMax - (offsetProgress * ButtonYesNoScaleMultiplier);
        WidgetNo->NormalSprite.BaseScale = glm::vec2(scaleNo);
        WidgetNo->FocusedSprite.BaseScale = glm::vec2(scaleNo);
        WidgetNo->MoveTo(glm::vec2(
            ButtonNoCenterPosX - (WidgetNo->NormalSprite.ScaledWidth() * 0.5f),
            ButtonNoCenterPosY -
                (WidgetNo->NormalSprite.ScaledHeight() * 0.5f)));
        WidgetNo->Tint = glm::vec4(1.0f, 1.0f, 1.0f,
                                   offsetProgress / ButtonYesNoAlphaDivider);
      } else {
        WidgetNo->Tint = glm::vec4(1.0f, 1.0f, 1.0f, 0.0f);
      }
    } else if (ChoiceCount == 1) {
      float offsetProgress =
          (animationProgress - AnimationProgressWidgetsStartOffset);
      float scaleOk =
          ButtonScaleMax - (offsetProgress * ButtonOKScaleMultiplier);
      WidgetOK->NormalSprite.BaseScale = glm::vec2(scaleOk);
      WidgetOK->FocusedSprite.BaseScale = glm::vec2(scaleOk);
      WidgetOK->MoveTo(glm::vec2(
          ButtonOKCenterPosX - (WidgetOK->NormalSprite.ScaledWidth() * 0.5f),
          ButtonOKCenterPosY - (WidgetOK->NormalSprite.ScaledHeight() * 0.5f)));
      WidgetOK->Tint =
          glm::vec4(1.0f, 1.0f, 1.0f, WidgetsAlphaMultiplier * offsetProgress);
    }
  }

  if (State != Hidden) {
    if (IsFocused) {
      MessageItems->Update(dt);
      ChoiceItems->Update(dt);
    }
  }
}

void SysMesBox::Render() {
  if (State != Hidden) {
    int animationFrame = std::ceil(FadeAnimation.Progress * AnimationSpeed);
    int totalSeals =
        animationFrame > SealSpriteCount ? SealSpriteCount : animationFrame;
    for (int i = 0; i < totalSeals; i++) {
      float alpha = glm::clamp(
          0.125f * ((FadeAnimation.Progress * AnimationSpeed) - i), 0.0f, 1.0f);
      glm::vec4 col(1.0f, 1.0f, 1.0f, alpha);
      float scale = (1.0f - alpha) + 1.0f;
      float width = SumoSealSprites[i].Bounds.Width * scale;
      float height = SumoSealSprites[i].Bounds.Height * scale;
      Renderer->DrawSprite(
          SumoSealSprites[i],
          RectF(SumoSealCenterPosX[i] - (width * 0.5f),
                SumoSealCenterPosY[i] - (height * 0.5f), width, height),
          col);
    }

    if (animationFrame > SealSpriteCount) {
      MessageItems->Tint.a =
          WidgetsAlphaMultiplier * ((FadeAnimation.Progress * AnimationSpeed) -
                                    AnimationProgressWidgetsStartOffset);
      MessageItems->Render();

      ChoiceItems->Render();
    }
  }
}

void SysMesBox::Init() {
  ChoiceMade = false;
  MessageCount = 0;
  ChoiceCount = 0;

  FadeAnimation.DurationIn = FadeInDuration;
  FadeAnimation.DurationOut = FadeOutDuration;
  FadeAnimation.Direction = 1;
  FadeAnimation.LoopMode = ALM_Stop;

  memset(Messages, 0, (8 * 255) * sizeof(ProcessedTextGlyph));
  memset(Choices, 0, (8 * 255) * sizeof(ProcessedTextGlyph));
}

void SysMesBox::AddMessage(uint8_t* str) {
  Impacto::Vm::Sc3VmThread dummy;
  dummy.Ip = str;
  int len = TextLayoutPlainLine(&dummy, 255, Messages[MessageCount],
                                Profile::Dialogue::DialogueFont, TextFontSize,
                                Profile::Dialogue::ColorTable[0], 1.0f,
                                glm::vec2(TextX, 0.0f), TextAlignment::Left);
  float mesLen = 0.0f;
  for (int i = 0; i < len; i++) {
    mesLen += Messages[MessageCount][i].DestRect.Width;
  }
  MessageWidths[MessageCount] = mesLen;
  MessageLengths[MessageCount] = len;
  MessageCount++;
}

void SysMesBox::AddChoice(uint8_t* str) {
  Impacto::Vm::Sc3VmThread dummy;
  dummy.Ip = str;
  int len = TextLayoutPlainLine(&dummy, 255, Choices[ChoiceCount],
                                Profile::Dialogue::DialogueFont, TextFontSize,
                                Profile::Dialogue::ColorTable[0], 1.0f,
                                glm::vec2(TextX, 0.0f), TextAlignment::Left);
  float mesLen = 0.0f;
  for (int i = 0; i < len; i++) {
    mesLen += Choices[ChoiceCount][i].DestRect.Width;
  }
  ChoiceWidths[ChoiceCount] = mesLen;
  ChoiceLengths[ChoiceCount] = len;
  ChoiceCount++;
}

}  // namespace CC
}  // namespace UI
}  // namespace Impacto